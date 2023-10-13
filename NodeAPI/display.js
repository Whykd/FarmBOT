const {LedMatrix, GpioMapping} = require('rpi-led-matrix');

function InsertNewData(newData , matrix){
  matrix.clear();
  //fill(x0: number, y0: number, x1: number, y1: number)
  //fgColor({ r: 255, g: 0, b: 0 })
  matrix.fgColor({r:0, g:0, b:0})
  .fill();
  for(let vals = 0; vals < 60; vals++){
    let avg = newData[vals]["Sensors"]["avg"]
    let sens1 = newData[vals]["Sensors"]["sens1"]
    let sens2 = newData[vals]["Sensors"]["sens2"]
    avg = ((avg * 32)/1024) + 1; //scale value to fit on 32 pixel height
    sens1 = ((sens1 * 32)/1024) + 1;
    sens2 = ((sens2 * 32)/1024) + 1;
    if (sens1 > sens2){
      //fills the matrix in order so they overlap correctly
      matrix.fgColor({ r: 255, g: 173, b: 66 })
      .fill(vals + 2, 0, vals + 2, sens1)
      .fgColor({ r: 164, g: 255, b: 66 })
      .fill(vals + 2, 0, vals + 2, avg)
      .fgColor({ r: 66, g: 135, b: 255 })
      .fill(vals + 2, 0, vals + 2, sens2)
    }
    else {
      matrix.fgColor({ r: 255, g: 173, b: 66 })
      .fill(vals + 2, 0, vals + 2, sens2)
      .fgColor({ r: 164, g: 255, b: 66 })
      .fill(vals + 2, 0, vals + 2, avg)
      .fgColor({ r: 66, g: 135, b: 255 })
      .fill(vals + 2, 0, vals + 2, sens1)
    }
  }
}

function initScreen(){
  let matrix = new LedMatrix(
    {
    ...LedMatrix.defaultMatrixOptions(),
    rows: 32,
    cols: 64,
    chainLength: 1,
    hardwareMapping: GpioMapping.AdafruitHat,
    },
    {
      ...LedMatrix.defaultRuntimeOptions(),
      gpioSlowdown: 4,
    }
  )
  return matrix;
}

module.exports = {
  InsertNewData,
  initScreen
}