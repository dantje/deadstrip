import xml.etree.ElementTree as ET
tree = ET.parse('map.xml')
root = tree.getroot()

datasym = []
textsym = []

print("digraph {")

for section in root.findall('FILE/SECTION'):
  symbol = section.get('name').replace('.text$','').replace('.data$','')
  for depends in section:
    if '.text$' in depends.text:
      depsym = depends.text.replace('.text$','')
      textsym.append(depsym)
    if '.data$' in depends.text:
      depsym = depends.text.replace('.data$','')
      datasym.append(depsym)
    
    print("  %s -> %s;" % (symbol,depsym))

for ts in set(textsym):
  print(' "%s" [shape=box];' % (ts))

for ds in set(datasym):
  print(' "%s" [shape=circle];' % (ds))

print("}")
