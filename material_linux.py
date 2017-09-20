import maya.cmds as cmds
import subprocess
import os.path

plasticName = 'Plastic'

def writeSceneFile(scenePath, resultPath):
    if os.path.isfile(scenePath):
        print 'deleting pbrt file ' + scenePath
        os.remove(scenePath)
    
    if os.path.isfile(resultPath):
        print 'deleting result image ' + resultPath
        os.remove(resultPath)
        
    # prepare values
    matVal = cmds.optionMenu(matOm, value = 1, q = 1)
    textureVal = cmds.optionMenu(textureOm, value = 1, q = 1).lower()
    matUseColor = cmds.checkBox(useColCb, value = 1, q = 1)
    matColVal = cmds.colorSliderGrp(matColCs, rgb = 1, q = 1)
    matUseKsColVal = cmds.checkBox(useKsColCb, value = 1, q = 1)
    matKsColVal = cmds.colorSliderGrp(matKsColCs, rgb = 1, q = 1)
    matUseTextVal = cmds.checkBox(useTextureCb, value = 1, q = 1)
    mainColVal = cmds.colorSliderGrp(mcCs, rgb = 1, q = 1)
    col1Val = cmds.colorSliderGrp(col1Cs, rgb = 1, q = 1)
    col2Val = cmds.colorSliderGrp(col2Cs, rgb = 1, q = 1)
    octVal = cmds.intField(octIf, value = 1, q = 1)
    roughVal = cmds.floatField(roughFf, value = 1, q = 1)
    colVarVal = cmds.floatField(colVarFf, value = 1, q = 1)
    scaleVal = cmds.floatField(scFf, value = 1, q = 1)
    remapRoughVal = cmds.checkBox(remapRoughCb, value = 1, q = 1)
    sigmaVal = cmds.floatField(sigmaFf, value = 1, q = 1)
    
    # create scene file
    sceneFile = open(scenePath, 'w')
    sceneFile.write('LookAt 2 2 5   0 -.4 0 0 1 0\n')
    sceneFile.write('Camera "perspective" "float fov" [22]\n')
    sceneFile.write('Film "image" "integer xresolution" [150] "integer yresolution" [150] "string filename" "')
    sceneFile.write(resultPath.replace('\\', '\\\\'))
    sceneFile.write('"\n')
    sceneFile.write('Integrator "directlighting" "integer maxdepth" [10]\n')
    sceneFile.write('Sampler "halton" "integer pixelsamples" [8]\n')
    sceneFile.write('WorldBegin\n')
    sceneFile.write('LightSource "infinite" "color L" [1 1 1]\n')
    sceneFile.write('Translate 0 -0.4 0\n')
    sceneFile.write('Texture "tex" "color" "')
    sceneFile.write(textureVal)
    sceneFile.write('" "integer octaves" [')
    sceneFile.write(str(octVal))
    sceneFile.write('] "float roughness" [')
    sceneFile.write(str(roughVal))
    sceneFile.write('] ')
    if textureVal == 'opal':
        sceneFile.write('"color mainCol" ')
        sceneFile.write(str(mainColVal).replace(',', ''))
        sceneFile.write(' "color col1" ')
        sceneFile.write(str(col1Val).replace(',', ''))
        sceneFile.write(' "color col2" ')
        sceneFile.write(str(col2Val).replace(',', ''))
        sceneFile.write(' "float colourVariant" [')
        sceneFile.write(str(colVarVal))
        sceneFile.write('] "float scale" [')
        sceneFile.write(str(scaleVal))
        sceneFile.write(']')
    sceneFile.write('\n')
    sceneFile.write('Material "')
    sceneFile.write(matVal.lower())
    sceneFile.write('" ')
    if matUseColor:
        sceneFile.write('"rgb Kd" ')
        sceneFile.write(str(matColVal).replace(',', ''))
        sceneFile.write(' ')
    if matUseKsColVal:
        sceneFile.write('"rgb Ks" ')
        sceneFile.write(str(matKsColVal).replace(',', ''))
        sceneFile.write(' ')
    if matUseTextVal:
        sceneFile.write('"texture Kd" "tex" ')
        if matVal == plasticName:
            sceneFile.write('"texture Ks" "tex" ')
    if matVal == plasticName:
        sceneFile.write('"float roughness" [')
        sceneFile.write(str(sigmaVal))
        sceneFile.write('] ')
        sceneFile.write('"bool remaproughness" ["')
        sceneFile.write(str(remapRoughVal).lower())
        sceneFile.write('"] ')
    else:
        sceneFile.write('"float sigma" [')
        sceneFile.write(str(sigmaVal))
        sceneFile.write('] ')
    sceneFile.write('\n')
    sceneFile.write('Shape "sphere"\n')
    sceneFile.write('WorldEnd\n')
    sceneFile.close()
    
def defaultButtonPush(*args):
    # get Path to PBRT executable
    sep = '/'
    pp = cmds.textField(pbrtPath, text=1, q=1)
    
    # check if the given file exists
    if not os.path.isfile(pp):
        print 'could not find pbrt executable at ' + pp
        return
        
    # prepare scene file
    dir = pp[:pp.rfind(sep)]
    scenePath = dir + sep +'opal.pbrt'
    # prepare output file
    resultPath = dir + sep +'opal.png'

    writeSceneFile(scenePath, resultPath)
    # Run PBRT    
    subprocess.call([pp, scenePath], stdout=subprocess.PIPE, stdin=subprocess.PIPE, stderr=subprocess.PIPE)
    
    # set the rendered image
    cmds.image(img, image = resultPath, e=1)

def choosePBRTLocation(*args):
    pp = cmds.textField(pbrtPath, text=1, q=1)
    sep = '/'

    newpbrtLocation = cmds.fileDialog(m = 0, dm = pp[:pp.rfind(sep)] + sep + '*')
    if newpbrtLocation:
        cmds.textField(pbrtPath, text=newpbrtLocation, e=1)
    
def changeMaterialVisibility(item):
    showPlastic = False
    height = 1
    sigmaName = 'Sigma'
    if item == plasticName:
        sigmaName = 'Roughness'
        showPlastic = True
        height = 20
    
    cmds.text(sigmaT1, label = sigmaName, e = 1)
    
    cmds.checkBox(remapRoughCb, visible = showPlastic, height = height, e = 1)
    cmds.text(remapRoughT1, visible = showPlastic, height = height, e = 1)
    
    cmds.checkBox(useKsColCb, value = False, visible = showPlastic, height = height, e = 1)
    cmds.colorSliderGrp(matKsColCs, visible = False, height = height, e = 1)
    
def changeMatColorVisibility(val):
    useCol = cmds.checkBox(useColCb, value = 1, q = 1)
    useText = cmds.checkBox(useTextureCb, value = 1, q = 1)
    useKsCol = cmds.checkBox(useKsColCb, value = 1, q = 1)
    if useText:
        useCol = False
        useKsCol = False
        cmds.checkBox(useColCb, value = False, e = 1)
        cmds.checkBox(useKsColCb, value = False, e = 1)

    cmds.colorSliderGrp(matColCs, visible = useCol, e = 1)
    cmds.colorSliderGrp(matKsColCs, visible = useKsCol, e = 1)
    
    height = 1
    showKd = False
    if not useText and not useCol:
        height = 20
        showKd = True

def changeKsColorVisibility(val):
    cmds.colorSliderGrp(matKsColCs, visible = val, e = 1)

def changeTextureVisibility(item):
    opalHeight = 1
    opalVis = False
    wrinkledHeight = 20
    wrinkledVis = True
	
    if item == 'Opal':
        opalHeight = 20
        opalVis = True
        wrinkledHeight = 1
        wrinkledVis = False

    cmds.colorSliderGrp(mcCs, visible = opalVis, height = opalHeight, e = 1)
    cmds.text(mcT1, visible = opalVis, height = opalHeight, e = 1)
    cmds.colorSliderGrp(col1Cs, visible = opalVis, height = opalHeight, e = 1)
    cmds.text(col1T1, visible = opalVis, height = opalHeight, e = 1)
    cmds.colorSliderGrp(col2Cs, visible = opalVis, height = opalHeight, e = 1)
    cmds.text(col2T1, visible = opalVis, height = opalHeight, e = 1)
    cmds.floatField(scFf, visible = opalVis, height = opalHeight, e = 1)
    cmds.text(scT1, visible = opalVis, height = opalHeight, e = 1)
    cmds.floatField(colVarFf, visible = opalVis, height = opalHeight, e = 1)
    cmds.text(colVarT1, visible = opalVis, height = opalHeight, e = 1)
    
def savePbrtSceneFile(*args):
    pp = cmds.textField(pbrtPath, text=1, q=1)
    sep = '/'
    location = cmds.fileDialog(m = 1, dm = pp[:pp.rfind(sep)] + sep + '*.pbrt', dfn = 'material.pbrt')
    if location:
        writeSceneFile(location, (location[location.rfind(sep)+1:location.rfind('.')] + '.exr'))
    
# Make a new window
window = cmds.window(title="Material Editor", iconName='Short Name', widthHeight=(300, 500))
cmds.rowColumnLayout(numberOfColumns=3, columnWidth=[(1, 100), (2, 150), (3, 40)])

cmds.text(label = 'Preview', align = 'left')
img = cmds.image(image = '', height = 150)
cmds.text(label = '', visible = False, height = 1)

cmds.text(label = 'Material', align = 'left')
matOm = cmds.optionMenu(changeCommand=changeMaterialVisibility)
cmds.menuItem(label = 'Matte')
cmds.menuItem(label = plasticName)
cmds.text(label = '', visible = False, height = 1)

useColCb = cmds.checkBox(label = 'Use Kd Colour', changeCommand=changeMatColorVisibility)
matColCs = cmds.colorSliderGrp(rgb = (1, 1, 1), visible = False)
cmds.text(label = '', visible = False, height = 1)

useKsColCb = cmds.checkBox(label = 'Use Ks Colour', changeCommand=changeMatColorVisibility, visible = False, height = 1)
matKsColCs = cmds.colorSliderGrp(rgb = (1, 1, 1), visible = False, height = 1)
cmds.text(label = '', visible = False, height = 1)

useTextureCb = cmds.checkBox(label = 'Use Texture', changeCommand=changeMatColorVisibility, value = True)
cmds.text(label = '', visible = False, height = 1)
cmds.text(label = '', visible = False, height = 1)

sigmaT1 = cmds.text(label = 'Sigma', align = 'left')
sigmaFf = cmds.floatField(minValue = 0, maxValue = 1)
cmds.text(label = '', visible = False, height = 1)

remapRoughT1 = cmds.text(label = 'Remap Roughness', align = 'left', visible = False, height = 1)
remapRoughCb = cmds.checkBox(label = '', value = True, visible = False, height = 1)
cmds.text(label = '', visible = False, height = 1)

cmds.text(label = 'Texture', align = 'left')
textureOm = cmds.optionMenu(changeCommand=changeTextureVisibility)
cmds.menuItem(label = 'Opal')
cmds.menuItem(label = 'Wrinkled')
cmds.text(label = '', visible = False, height = 1)

mcT1 = cmds.text(label = 'Main Colour', align = 'left')
mcCs = cmds.colorSliderGrp(rgb = (147.0 / 255.0, 160.0 / 255.0, 223.0 / 255.0))
cmds.text(label = '', visible = False, height = 1)

col1T1 = cmds.text(label = 'Colour 1', align = 'left')
col1Cs = cmds.colorSliderGrp(rgb = (240.0 / 255.0, 162.0 / 255.0, 177.0 / 255.0))
cmds.text(label = '', visible = False, height = 1)

col2T1 = cmds.text(label = 'Colour 2', align = 'left')
col2Cs = cmds.colorSliderGrp(rgb = (132.0 / 255.0, 235.0 / 255.0, 162.0 / 255.0))
cmds.text(label = '', visible = False, height = 1)

octT1 = cmds.text(label = 'Octaves', align = 'left')
octIf = cmds.intField(minValue = 1, maxValue = 512, value = 8)
cmds.text(label = '', visible = False, height = 1)

roughT1 = cmds.text(label = 'Roughness', align = 'left')
roughFf = cmds.floatField(minValue = 0, maxValue = 1, value = 0.5)
cmds.text(label = '', visible = False, height = 1)

scT1 = cmds.text(label = 'Scale', align = 'left')
scFf = cmds.floatField(minValue = 0.1, maxValue = 10, value = 4)
cmds.text(label = '', visible = False, height = 1)

colVarT1 = cmds.text(label = 'Colour variant', align = 'left')
colVarFf = cmds.floatField(minValue = 0, maxValue = 1, value = 0.5)
cmds.text(label = '', visible = False, height = 1)

cmds.text(label = 'PBRT Path', align = 'left')
pbrtPath = cmds.textField(text='/home/treierflor/cgra408/proj2/build/pbrt')
cmds.button(label='...', command=choosePBRTLocation)

cmds.text(label = '', visible = False, height = 1)
cmds.button(label='Render', command=defaultButtonPush)
cmds.text(label = '', visible = False, height = 1)

cmds.text(label = '', visible = False, height = 1)
cmds.button(label='Save PBRT Scene File', command=savePbrtSceneFile)
cmds.text(label = '', visible = False, height = 1)

cmds.setParent('..')
cmds.showWindow(window)