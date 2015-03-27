import xml.etree.ElementTree as ET
tree = ET.parse('map.xml')
root = tree.getroot()

print("digraph {")

for section in root.findall('FILE/SECTION'):
  symbol = section.get('name').replace('.text$','').replace('.data$','')
  for depends in section:
    depsym = depends.text.replace('.text$','').replace('.data$','')
    print("  %s -> %s;" % (symbol,depsym))

print("}")
