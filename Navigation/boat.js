const {Vector,CartesianVector,PolarVector}=require("./vector.js");
const {Component,Motor,Battery}=require("./components.js");
const {ceil,distance}=require("./utils.js");

class Boat {
    constructor(simulation) {
        this.simulation = simulation;
        this.position = {...simulation.boatStartingPoint};
        this.velocity = new CartesianVector(0, 0);
        this.acceleration = new CartesianVector(0, 0);
        this.speed = 0;
        this.waypoints = [];
        this.originalWaypoints = [];
        this.heading = {...simulation.targetPosition}
        this.squaredDistance = 0;
        this.color = Math.random() * 360;//80 + Math.random() * 200;
        this.area = 1;
        this.arrived = false;
        this.arrivedAt=Infinity;
        this.crashed = false;
        this.components = [];
        this.hullMass = 3000;
        this.mass = 0;
        this.thrust = 500;
        this.distanceToEnd = Infinity;
        this.positionHistory = [];
    }
    init() {
        var waypointsN = 2
        for (var i = 0; i < waypointsN; i++) {
            var waypoint = { x: 0, y: 0 };
            var spread = 2;
            waypoint.x = this.position.x + (this.heading.x - this.position.x) * i / waypointsN + Math.random() * this.simulation.worldWidth / spread - (this.simulation.worldWidth / spread) / 2;
            waypoint.y = this.position.y + (this.heading.y - this.position.y) * i / waypointsN + Math.random() * this.simulation.worldHeight / spread - (this.simulation.worldHeight / spread) / 2;
            this.waypoints.push(waypoint);
        }
        this.waypoints.push({ ...this.heading })
        this.originalWaypoints = [...this.waypoints]
        this.components.push(new Motor("pod-drive-1.0-evo"));
        this.components.push(new Battery("vertex-s-trina-solar"));
    }
    initFromParents(a, b) {
        var waypointsN = a.originalWaypoints.length - 1;
        for (var i = 0; i < waypointsN; i++) {
            var waypoint = { x: 0, y: 0 };
            var spread = 2;
            if (Math.random() < 0.2) {
                waypoint.x = (a.originalWaypoints[i].x * a.matingScore + b.originalWaypoints[i].x * b.matingScore) / (a.matingScore + b.matingScore);
            } else if (Math.random() < 0.5) {
                waypoint.x = a.originalWaypoints[i].x;
            } else {
                waypoint.x = b.originalWaypoints[i].x;
            }
            if (Math.random() < 0.2) {
                waypoint.y = (a.originalWaypoints[i].y * a.matingScore + b.originalWaypoints[i].y * b.matingScore) / (a.matingScore + b.matingScore);
            } else if (Math.random() < 0.5) {
                waypoint.y = a.originalWaypoints[i].y;
            } else {
                waypoint.y = b.originalWaypoints[i].y;
            }
            this.waypoints.push(waypoint);
        }
        this.waypoints.push({ ...this.heading })
        this.originalWaypoints = [...this.waypoints]
        this.components.push(new Motor("pod-drive-1.0-evo"));
        this.components.push(new Battery("vertex-s-trina-solar"));
    }
    act(deltaTime, current) {
        var C = 1;
        var availablePower = 0;
        var sumPriorities = 0;
        this.mass = this.hullMass;
        this.components.forEach((component) => {
            this.mass += component.mass;
            availablePower += component.outputPower;
            if (component.priority === 1) {
                var usedPower = Math.min(availablePower, component.inputPower);
                availablePower -= usedPower;
                component.availablePower = usedPower;
            } else if (component.inputPower) {
                sumPriorities += component.priority + 1;
            }
        });
        this.components.forEach(component => {
            if (component.priority < 1) {
                var usedPower = Math.min(availablePower, component.inputPower * (component.priority + 1) / sumPriorities);
                component.availablePower = usedPower;
                availablePower -= usedPower;
            }
        })
        this.thrust = 0;
        this.components.forEach(component => {
            if (component.thrust) {
                component.actualThrust = component.thrust * component.availablePower / component.inputPower;
                this.thrust += component.actualThrust;
            }
        })
        var currentForce = 1 / 2 * this.simulation.consts.waterDensity * (Math.pow(current.magnitude, 2)) * this.area;
        this.direction = Math.atan2(this.waypoints[0].y - this.position.y, this.waypoints[0].x - this.position.x);
        var drag = new Vector(1 / 2 * this.simulation.consts.waterDensity * C * this.area * Math.pow(this.velocity.magnitude, 2), this.velocity.direction + Math.PI);
        drag.magnitude *= (1 + Math.abs(Math.sin(Math.atan2(Math.sin(this.direction - this.velocity.direction), Math.cos(this.direction - this.velocity.direction)))));
        this.acceleration = {
            x: Math.cos(this.direction) * this.thrust * 1 / this.mass + 1 * Math.cos(current.direction) * currentForce / this.mass + drag.x / this.mass,
            y: Math.sin(this.direction) * this.thrust * 1 / this.mass + 1 * Math.sin(current.direction) * currentForce / this.mass + drag.y / this.mass
        };
        if (isNaN(this.acceleration.x)) {
            console.log("NaN");
        }
        this.velocity.x += this.acceleration.x * deltaTime / 1000;
        this.velocity.y += this.acceleration.y * deltaTime / 1000;
        this.position.x += this.velocity.x * deltaTime / 1000;
        this.position.y += this.velocity.y * deltaTime / 1000;
        if (this.position.x <= 0) this.position.x = 0;
        if (this.position.y <= 0) this.position.y = 0;
        if (this.position.x >= this.simulation.worldWidth - this.simulation.scale) this.position.x = this.simulation.worldWidth - this.simulation.scale - 1;
        if (this.position.y >= this.simulation.worldHeight - this.simulation.scale) this.position.y = this.simulation.worldHeight - this.simulation.scale - 1;
        var distanceToNextWaypoint = distance(this.position, this.waypoints[0]);
        while (this.waypoints.length > 1 && distanceToNextWaypoint > distance(this.position, this.waypoints[1])) {
            this.waypoints.splice(0, 1);
            distanceToNextWaypoint = distance(this.position, this.waypoints[0]);
        }
        this.positionHistory.push({ ...this.position });
        this.distanceToEnd = Math.sqrt(Math.pow(this.position.x - this.simulation.worldWidth / 1.5, 2) + Math.pow(this.position.y - this.simulation.worldHeight / 1.5, 2))
    }
}
module.exports=Boat;