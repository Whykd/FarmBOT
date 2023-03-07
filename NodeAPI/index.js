const express = require("express");
const cors = require("cors");
const { MongoClient } = require("mongodb");
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
const port = new SerialPort({ path: sport, baudRate: 576000 }, function (err) {
	if (err) {
		return console.log("Error: ", err.message);
	}
});
const parser = port.pipe(new ReadlineParser({ delimiter: "\r\n" }));
const PORT = 4000;
app.use(express.json());
app.use(cors());
port.setDefaultEncoding("utf8");
app.listen(PORT, () => {
	console.log(`Server running on port ${PORT}`);
});

app.post("/update", async (req, res) => {
	const pf = req.body.passphrase;
	const data = JSON.stringify(req.body.data);
	let ln = 0;
	if (data) {
		ln = data.split("[")[1].split("]")[0].split(",").length;
	}

	if (pf == "PresFarmbotFOREVER") {
		if (ln == 5) {
			const output = 1 + data.substring(1, data.length - 1);
			port.write(output);
			checkReponse().then((result) => {
				if (result == 0) {
					//console.log("No Data Recieved By Arduino")
					res.status(503).send("No Data Recieved By Arduino");
					return;
				}
				if (result == 1) {
					//console.log("Data Recieved By Arduino")
					res.status(200).send("Data Recieved By Arduino");
					return;
				} else if (result == 2) {
					//console.log("Bad Response From Arduino")
					res.status(500).send("Bad Response From Arduino");
					return;
				} else {
					//console.log("Internal Server Error 1")
					res.status(500).send("Internal Server Error 1");
					return;
				}
			});
		} else {
			//console.log("Bad Data")
			res.status(417).send("Bad Data");
			return;
		}
	} else {
		//console.log("Incorect Passphrase")
		res.status(401).send("Incorect Passphrase");
		return;
	}
});

function checkReponse() {
	return new Promise((resolve, reject) => {
		parser.on("data", (data) => {
			if (data == "OK") {
				resolve(1);
			} else {
				resolve(2);
			}
		});
	});
}

parser.on("data", (data) => {
	if (data.substring(0, 1) == "[" && data.substring(data.length - 1) == "]") {
		const sens1 = parseInt(data.split("[")[1].split("]")[0].split(",")[0]);
		const sens2 = parseInt(data.split("[")[1].split("]")[0].split(",")[1]);
		const avg = (parseInt(sens1) + parseInt(sens2)) / 2;
		db.collection("sensdata").insertOne({
			sens1: sens1,
			sens2: sens2,
			avg: avg,
			timestamp: new Date(),
		});
		//create a new document in the collection sensdata with sens1 and sens2 and the current time as the timestamp
	}
});
