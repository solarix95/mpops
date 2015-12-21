sceneInit(800,600)

background = shapeCreateByMedia("background.png")
lena               = shapeCreateByMedia("playmate_640.jpg")
shapeScale(lena,0.1,0.1,1)

shapeSetPos(background,0,0,700)


-- RENDER
shapeSetPos(lena,0,0,0)
shapeRotate(lena,30,30,0)

