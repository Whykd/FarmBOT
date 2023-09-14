const express = require("express");
const cors = require("cors");
const { MongoClient } = require("mongodb");
const schedule = require('node-schedule');
require("dotenv").config();
const uri = process.env.URI;
const sport = process.env.SPORT;
//console.log(uri)
const client = new MongoClient(uri);
try {
	client.connect();
} catch (e) {
	console.error(e);
}
const db = client.db("sensdata");
const app = express();
//db.collection('sensdata').deleteMany({});
const { SerialPort } = require("serialport");
const { ReadlineParser } = require("@serialport/parser-readline");
const port = new SerialPort({ path: sport, baudRate: 57600 }, function (err) {
	if (err) {
		return console.log("Error: ", err.message);
	}
});
const parser = port.pipe(new ReadlineParser({ delimiter: "\r\n" }));
const PORT = 4000;

let ardCounter = 0;
// let bucketHasWater = false;

app.use(express.json());
app.use(cors());
port.setDefaultEncoding("utf8");
app.listen(PORT, () => {
	console.log(`Server running on port ${PORT}`);
});

app.post("/update", async (req, res) => {
	const pf = req.body.passphrase;
	const data = req.body.data;

	const watertHour = data.WaterHour
	const waterStartMin = data.waterStartMin
	const waterEndMin = data.waterEndMin
	const lightStartHour = data.lightStartHour
	const lightEndHour = data.lightEndHour

	dataOut = {
		wh : watertHour,
		wsm : waterStartMin,
		wem : waterEndMin,
		lsh : lightStartHour,
		leh : lightEndHour
	}

	if (pf == process.env.PASSPHRASE){
		port.write(dataOut)

	}


	// let ln = 0;
	// if (data) {
	// 	ln = data.split("[")[1].split("]")[0].split(",").length;
	// }

	// if (pf == process.env.PASSPHRASE) {
	// 	if (ln == 5) {
	// 		const output = 1 + data.substring(1, data.length - 1);
	// 		port.write(output);
	// 		checkReponse().then((result) => {
	// 			if (result == 0) {
	// 				//console.log("No Data Recieved By Arduino")
	// 				res.status(503).send("No Data Recieved By Arduino");
	// 				return;
	// 			}
	// 			if (result == 1) {
	// 				//console.log("Data Recieved By Arduino")
	// 				res.status(200).send("Data Recieved By Arduino");
	// 				return;
	// 			} else if (result == 2) {
	// 				//console.log("Bad Response From Arduino")
	// 				res.status(500).send("Bad Response From Arduino");
	// 				return;
	// 			} else {
	// 				//console.log("Internal Server Error 1")
	// 				res.status(500).send("Internal Server Error 1");
	// 				return;
	// 			}
	// 		});
	// 	} else {
	// 		//console.log("Bad Data")
	// 		res.status(417).send("Bad Data");
	// 		return;
	// 	}
	// } else {
	// 	//console.log("Incorect Passphrase")
	// 	res.status(401).send("Incorect Passphrase");
	// 	return;
	// }
});

app.put("/data", async (req, res) => {
	dataOut = db.collection("sensdata").find().limit(60)
	res.send(dataOut)
})



parser.on("data", (data) => {
	ardCounter++
	const sens1 = data.sens1
	const sens2 = data.sens2
	const avg = (parseInt(sens1) + parseInt(sens2)) / 2;
	db.collection("sensdata").insertOne
	({
		sens1: sens1,
		sens2: sens2,
		avg: avg,
		timestamp: new Date(),
	});
	const hasWater = data.hasWater

	const curHour = data.curHour
	const curMin = data.curMin

	if (curHour != new Date().getHours() || curMin != new Date().getMinutes()){
		syncClock()
	}

});
function syncClock() {
	const date = new Date();
	const hour = date.getHours();
	const min = date.getMinutes();
	const output = {
		hour : hour,
		min : min
	}
	port.write(output);
}
const job = schedule.scheduleJob('1 * * * *', () => { // run every hour at minute 1
    if (ardCounter < 2){
		port.close();
		port.open();
	}
	ardCounter = 0;
});