const GeneticAlgorithm = require("./geneticAlgorithm.js");
const fs=require("fs");
const prompt = require("prompt-sync")({ sigint: true });

function distance(a, b) {
    return Math.sqrt(Math.pow(a.x - b.x, 2) + Math.pow(a.y - b.y, 2));
}

// var latStart = prompt("Point de départ - latitude : ");
// var longStart = prompt("Point de départ - longitude : ");
// var latEnd = prompt("Point d'arrivée - latitude : ");
// var longEnd = prompt("Point d'arrivée - longitude : ");

var latStart = 48.891123;
var longStart = 2.225268;
var latEnd = 48.894442;
var longEnd = 2.216091;

class Navigator {
    constructor() {
        this.translation = null;
        this.scaleFactor = null;
    }
    init(latStart, longStart, latEnd, longEnd) {
        var scaleFactor = (6000 / (((latStart - latEnd) ** 2 + (longStart - longEnd) ** 2) ** (1 / 2)));
        var scaledStart = { x: latStart * scaleFactor, y: longStart * scaleFactor };
        var scaledEnd = { x: latEnd * scaleFactor, y: longEnd * scaleFactor };

        var translation = { x: -scaledStart.x, y: -scaledStart.y }

        var translatedStart = { x: 0, y: 0 }
        var translatedEnd = { x: scaledEnd.x + translation.x, y: scaledEnd.y + translation.y };

        if (translatedEnd.x < 0) {
            translation.x -= translatedEnd.x;
            translatedStart.x -= translatedEnd.x;
            translatedEnd.x = 0;
        }
        if (translatedEnd.y < 0) {
            translation.y -= translatedEnd.y;
            translatedStart.y -= translatedEnd.y;
            translatedEnd.y = 0;
        }

        var meanX = (translatedStart.x + translatedEnd.x) / 2;
        var meanY = (translatedStart.y + translatedEnd.y) / 2;

        var finaldX = 4000 - meanX;
        var finaldY = 4000 - meanY;

        translation.x += finaldX;
        translation.y += finaldY;
        this.start = { x: translatedStart.x + finaldX, y: translatedStart.y + finaldY };
        this.end = { x: translatedEnd.x + finaldX, y: translatedEnd.y + finaldY };
        this.scaleFactor = scaleFactor;
        this.translation = translation;

        this.GA = new GeneticAlgorithm(this.start, this.end);
        this.GA.initialize(200);
        this.bestBoat = null;
        this.waypoints = [];
    }
    convertLatLongToCart(latLong) {
        return { x: (latLong.lat * this.scaleFactor) + this.translation.x, y: (latLong.long * this.scaleFactor) + this.translation.y };
    }
    convertCartToLatLong(pos) {
        return { lat: (pos.x - this.translation.x) / this.scaleFactor, long: (pos.y - this.translation.y) / this.scaleFactor }
    }
    async findBestItinerary() {
        console.log("Finding best itinerary");
        var n = 10;
        for (var i = 0; i < n; i++) {
            console.log(Math.floor(i / n * 100) + "%");
            await this.run();
        }
        console.log("Done");
        this.GA.bestBoatHistory.sort((a, b) => a.arrivedAt - b.arrivedAt);
        this.bestBoat = this.GA.bestBoatHistory[0];
        this.waypoints = [...this.bestBoat.originalWaypoints];
        console.log("Next waypoint : ", this.convertCartToLatLong(this.waypoints[0]));
        var data=this.waypoints.map(p => this.convertCartToLatLong(p)).map(p=>p.lat+";"+p.long).join(",")+"|"+this.bestBoat.positionHistory.map(p => this.convertCartToLatLong(p)).filter(e => Math.random() < 50 / this.bestBoat.positionHistory.length).map(p=>p.lat+";"+p.long).join(",");
        var url=new URL("file:///"+__dirname+"/test.html");
        url.searchParams.set("data",data);
        fs.writeFileSync("url.txt",url.href);
        // console.log(JSON.stringify(this.waypoints.map(p => this.convertCartToLatLong(p))));
        // console.log(JSON.stringify(this.bestBoat.positionHistory.map(p => this.convertCartToLatLong(p)).filter(e => Math.random() < 50 / this.bestBoat.positionHistory.length)))
    }
    async run() {
        await this.GA.calculateFitting();
        this.GA.selection();
        this.GA.crossover();
        this.GA.mutate();
        this.GA.reset();
        return;
    }
    isOnTrack(pos) {
        pos = this.convertLatLongToCart(pos);
        var bb = this.bestBoat;
        var minDist = Infinity;
        var closestPoint = null;
        for (var i = 0; i < bb.positionHistory.length; i++) {
            var dist = distance(bb.positionHistory[i], pos);
            if (dist < minDist) {
                minDist = dist;
                closestPoint = bb.positionHistory[i];
            }
        }
        if (distance(pos, this.waypoints[0]) < 100) {
            console.log("Waypoint validated");
            this.waypoints.splice(0, 1);
            console.log("Next waypoint : ", this.convertCartToLatLong(this.waypoints[0]));
        }
        if (minDist < 200) {
            console.log("OK")
            return true;
        } else {
            console.log("Not OK");
            return false;
        }
    }
    reset(start) {
        this.GA = new GeneticAlgorithm(start,);
    }
}

async function main() {
    var nav = new Navigator();
    nav.init(latStart, longStart, latEnd, longEnd)
    await nav.findBestItinerary();
    var currentPosition = { lat: 0, long: 0 };
    while (1) {
        var currentPositionPrompt = prompt("Point actuel latitude;longitude : ").split(";")
        currentPosition = { lat: parseFloat(currentPositionPrompt[0]), long: parseFloat(currentPositionPrompt[1]) };
        if (!(currentPosition.lat && currentPosition.long)) break;
        if (!(nav.isOnTrack(currentPosition))) break;
    }
    latStart = currentPosition.lat;
    longStart = currentPosition.long;
    main();
}

main();