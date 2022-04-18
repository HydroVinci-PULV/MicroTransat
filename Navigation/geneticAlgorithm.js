const Boat = require("./boat.js");
const Simulation = require("./simulation.js");
const { ceil, distance } = require("./utils.js");
class GeneticAlgorithm {
    constructor(origin={x:2000,y:2000},destination = { x: 5000, y: 5000 }) {
        this.population = [];
        this.origin={x:0,y:0};
        this.destination=destination;
        this.simulation = new Simulation(8000, 8000, 3, origin,destination)
        this.currentBestBoat=null;
        this.bestBoatHistory=[];
    }
    initialize(populationSize = 100) {
        this.population = [];
        for (var i = 0; i < populationSize; i++) {
            this.population.push(new Boat(this.simulation));
            this.population[i].init();
        }
        this.simulation.boats = this.population;
    }
    async calculateFitting() {
        var temoin = new Boat(this.simulation);
        temoin.o = true;
        temoin.init();
        temoin.waypoints.splice(0, 2);
        this.simulation.boats.push(temoin);

        await this.simulation.compute();
        var maxFittingScore = -Infinity;
        var bestBoat=null;
        this.population.forEach(boat => {
            boat.fittingScore = 1 / distance(this.simulation.targetPosition, boat.position) / Math.pow((boat.arrivedAt || this.simulation.time), 2);
            if (boat.fittingScore > maxFittingScore) {
                maxFittingScore = boat.fittingScore;
                bestBoat=boat;
            }
        })
        this.currentBestBoat=bestBoat;
        this.bestBoatHistory.push(bestBoat);
        this.population.forEach(boat => boat.fittingScore *= 1 / maxFittingScore);
    }
    selection() {
        this.matingPool = [];
        for (var i = 0; i < this.population.length; i++) {
            this.population.forEach(boat => { boat.matingScore = Math.pow(boat.fittingScore, 0.3) * Math.random() });
            this.population.sort((a, b) => b.matingScore - a.matingScore);
            this.matingPool.push(this.population[0], this.population[1]);
        }
        var sum = 0;
        this.matingPool.forEach(boat => sum += boat.fittingScore);
        var mean = sum / this.matingPool.length
        var sumSTD = 0;
        this.matingPool.forEach(boat => sumSTD += Math.pow(boat.fittingScore - mean, 2));
        var STD = Math.sqrt(sumSTD / this.matingPool.length);
        // console.log("Mean :", mean, "STD :", STD);
    }
    crossover() {
        this.children = [];
        for (var i = 0; i < this.population.length; i++) {
            var boat = new Boat(this.simulation);
            boat.initFromParents(this.matingPool[i * 2], this.matingPool[i * 2 + 1]);
            this.children.push(boat);
        }
    }
    mutate() {
        this.children.forEach(boat => {
            if (Math.random() < 0.2) {
                boat.waypoints[Math.floor((boat.waypoints.length - 1) * Math.random())].x += Math.random() * 100 - 50;
            }
            if (Math.random() < 0.2) {
                boat.waypoints[Math.floor((boat.waypoints.length - 1) * Math.random())].y += Math.random() * 100 - 50;
            }
            if (Math.random() < 0.05) {
                boat.waypoints[Math.floor((boat.waypoints.length - 1) * Math.random())].x = Math.random() * this.simulation.width;
            }
            if (Math.random() < 0.05) {
                boat.waypoints[Math.floor((boat.waypoints.length - 1) * Math.random())].y = Math.random() * this.simulation.height;
            }
        })
    }
    reset() {
        this.population = this.children;
        this.simulation.reset();
        this.simulation.boats = this.population;
    }
}
module.exports = GeneticAlgorithm