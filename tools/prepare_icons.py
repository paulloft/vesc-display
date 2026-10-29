#!/usr/bin/env python3
"""Slice the generated sheets, normalize visible bounds and export LVGL rasters.

The black export background becomes the alpha channel. No icon contours are
redrawn: only crop, mirror, resize and placement in fixed transparent cells.
"""
from pathlib import Path
import json
from PIL import Image, ImageChops, ImageOps

root = Path(__file__).resolve().parents[1]
assets = root/'assets/icons'
sheet = Image.open(assets/'source-sheet.png').convert('RGB')
arrows = Image.open(assets/'source-arrows.png').convert('RGB')
roles = ['left','right','position','low_beam','high_beam','hazard','warning','battery','thermometer']

def mask(source, box):
    tile=source.crop(box)
    r,g,b=tile.split()
    alpha=ImageChops.lighter(ImageChops.lighter(r,g),b)
    alpha=alpha.point(lambda value: 0 if value<12 else min(255,round((value-12)*255/243)))
    bounds=alpha.point(lambda value:255 if value>=48 else 0).getbbox()
    assert bounds, 'Missing icon'
    return alpha.crop(bounds)

icons={}
for i,name in enumerate(roles):
    col,row=i%3,i//3
    icons[name]=mask(sheet,(round(col*sheet.width/3),round(row*sheet.height/3),round((col+1)*sheet.width/3),round((row+1)*sheet.height/3)))
icons['left']=mask(arrows,(0,0,arrows.width//2,arrows.height))
icons['right']=ImageOps.mirror(icons['left'])
brake=Image.open(assets/'source-brake.png').convert('RGB')
icons['brake']=mask(brake,(0,0,brake.width,brake.height))
roles.append('brake')

source=['#include <lvgl.h>','']
header=['#pragma once','#include <lvgl.h>','']
manifest={}
for name,alpha in icons.items():
    canvas_size=(28,48) if name=='battery' else (24,42) if name=='thermometer' else (64,56) if name=='hazard' else (36,28)
    content_size=(22,44) if name=='battery' else (18,36) if name=='thermometer' else (48,48) if name=='hazard' else (22,22) if name=='warning' else (30,24)
    resized=ImageOps.contain(alpha,content_size,Image.Resampling.LANCZOS)
    canvas=Image.new('RGBA',canvas_size,(255,255,255,0))
    centered=Image.new('L',canvas_size,0)
    at=((canvas.width-resized.width)//2,(canvas.height-resized.height)//2)
    centered.paste(resized,at);canvas.putalpha(centered)
    canvas.save(assets/(name+'.png'))
    canvas.save(root/'ebike_display/images'/(name+'.png'))
    bounds=centered.point(lambda value:255 if value>=32 else 0).getbbox()
    assert bounds
    center=((bounds[0]+bounds[2]-1)/2,(bounds[1]+bounds[3]-1)/2)
    desired=((canvas.width-1)/2,(canvas.height-1)/2)
    assert max(abs(a-b) for a,b in zip(center,desired))<=0.5,(name,bounds,center,desired)
    manifest[name]={'size':canvas_size,'visible_bounds':bounds,'center':center,'cell_center':desired}
    # A8 mask: LVGL 9 recolors the raster without allocating RGBA layers.
    data=bytearray()
    for value in centered.get_flattened_data():data.append(value)
    source.append(f'static const uint8_t pixels_{name}[] = {{')
    source.extend(','.join(str(value) for value in data[i:i+36])+',' for i in range(0,len(data),36))
    source.extend(['};',f'const lv_image_dsc_t icon_{name} = {{',
      f' .header = {{.magic=LV_IMAGE_HEADER_MAGIC,.cf=LV_COLOR_FORMAT_A8,.w={canvas.width},.h={canvas.height},.stride={canvas.width}}},',
      f' .data_size=sizeof(pixels_{name}), .data=pixels_{name}', '};',''])
    header.append(f'LV_IMAGE_DECLARE(icon_{name});')
(root/'src/icon_assets.c').write_text('\n'.join(source))
(root/'include/IconAssets.h').write_text('\n'.join(header)+'\n')
(assets/'manifest.json').write_text(json.dumps(manifest,indent=2)+'\n')
# An inspection sheet at native screen scale and 4x, on the dashboard color.
preview=Image.new('RGB',(360,56*((len(roles)+2)//3)),'#07121c')
for i,name in enumerate(roles):
    im=Image.open(assets/(name+'.png'))
    preview.paste(im,(i%3*120+(120-im.width)//2,i//3*56+(56-im.height)//2),im)
preview.resize((preview.width*4,preview.height*4),Image.Resampling.NEAREST).save(assets/'preview.png')
print(f'Prepared {len(roles)} raster icons; visible bounds centered to within half a pixel.')

print("LVGL Pro images updated. Run Export code only before building.")
