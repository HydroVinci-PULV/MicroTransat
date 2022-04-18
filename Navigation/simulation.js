const Boat=require("./Boat.js");
const noise =require("./perlin.js").noise;
noise.seed(-1201.2);
const {Vector,CartesianVector,PolarVector}=require("./vector.js");
const {ceil,distance}=require("./utils.js");
class Simulation {
    constructor(worldWidth, worldHeight, mapScale,origin={x:0,y:0}, targetPosition = { x: 0, y: 0 }) {
        this.reset();
        this.scale = worldWidth / 50;
        this.width = worldWidth / this.scale;
        this.height = worldHeight / this.scale;
        this.mapScale = mapScale;
        this.worldWidth = worldWidth;
        this.worldHeight = worldHeight;
        this.timeSpeed = 10000;
        this.consts = { waterDensity: 1 }
        this.currentsMap = [];
        this.targetPosition = targetPosition;
        this.boatStartingPoint=new CartesianVector(origin.x,origin.y);
        this.initCurrentsMap();
        this.actCurrents();
        this.hasArrived = false;
        this.closestBoat = null;
    }
    reset() {
        this.time = 0;
        this.arrived = 0;
        this.boats = [];
    }
    initCurrentsMap() {
        for (var j = 0; j < this.height; j++) {
            this.currentsMap[j] = [];
            for (var i = 0; i < this.width; i++) {
                this.currentsMap[j][i] = 0;
            }
        }
    }
    actCurrents() {
        for (var i = 0; i < this.width; i++) {
            this.currentsMap[i] = [];
            for (var j = 0; j < this.height; j++) {
                this.currentsMap[i][j] = new Vector((noise.perlin3(i / this.width * this.mapScale, j / this.height * this.mapScale, this.time / 10000000) + 1) / 2 * 8, (noise.perlin3(i / this.width * this.mapScale + 100.012201, j / this.height * this.mapScale + 100 + 0.0232, this.time / 10000000) + 1) / 2 * Math.PI * 2);
            }
        }
    }
    act() {
        this.closestBoat = null;
        this.time +=this.timeSpeed;
        this.actCurrents();
        for (var i = 0; i < this.boats.length; i++) {
            var boat = this.boats[i];
            if (boat.arrived || boat.crashed) continue;
            try {
                var aData = { x: Math.floor(boat.position.x / this.scale), y: Math.floor(boat.position.y / this.scale) }
                var bData = { x: ceil(boat.position.x / this.scale), y: Math.floor(boat.position.y / this.scale) };
                var cData = { x: Math.floor(boat.position.x / this.scale), y: ceil(boat.position.y / this.scale) };
                var dData = { x: ceil(boat.position.x / this.scale), y: ceil(boat.position.y / this.scale) };
                aData.current = this.currentsMap[aData.x][aData.y];
                bData.current = this.currentsMap[bData.x][bData.y];
                cData.current = this.currentsMap[cData.x][cData.y];
                dData.current = this.currentsMap[dData.x][dData.y];
                dData.coeff = (boat.position.x / this.scale - aData.x) * (boat.position.y / this.scale - aData.y);
                cData.coeff = (bData.x - boat.position.x / this.scale) * (boat.position.y / this.scale - bData.y);
                bData.coeff = (boat.position.x / this.scale - cData.x) * (cData.y - boat.position.y / this.scale);
                aData.coeff = (dData.x - boat.position.x / this.scale) * (dData.y - boat.position.y / this.scale);
                var current = new Vector();
                var sumSin = Math.sin(aData.current.direction) * aData.coeff + Math.sin(bData.current.direction) * bData.coeff + Math.sin(cData.current.direction) * cData.coeff + Math.sin(dData.current.direction) * dData.coeff;
                var sumCos = Math.cos(aData.current.direction) * aData.coeff + Math.cos(bData.current.direction) * bData.coeff + Math.cos(cData.current.direction) * cData.coeff + Math.cos(dData.current.direction) * dData.coeff;
                current.direction = Math.atan2(sumSin, sumCos);
                current.magnitude = aData.current.magnitude * aData.coeff + bData.current.magnitude * bData.coeff + cData.current.magnitude * cData.coeff + dData.current.magnitude * dData.coeff;
                if (current.magnitude > 100) {
                    console.log(200);
                }
                boat.act(this.timeSpeed, current);
                if (boat.position.y > this.worldHeight - this.scale) {
                    // console.log("outside");
                }
                if (((boat.position.x - boat.waypoints[0].x) ** 2 + (boat.position.y - boat.waypoints[0].y) ** 2) ** (1 / 2) < 100) {
                    boat.waypoints.splice(0, 1);
                }
                if (boat.waypoints.length == 0 ||distance(boat.position,boat.heading)<100) {
                    if (!this.hasArrived) {
                        this.timeArrived = this.time;
                    }
                    this.hasArrived = true;
                    boat.arrived = true;
                    boat.arrivedAt = this.time;
                    this.arrived++;
                    // console.log("Arrived");
                }
                if (boat.distanceToEnd < this.minDistance) {
                    this.minDistance = boat.distanceToEnd;
                    this.closestBoat = boat;
                }

                boat.positionHistory.push({ x: boat.position.x, y: boat.position.y });
            } catch (e) {
                throw (e);
                boat.crashed = true;
                console.log("crashed")
            }
        }
        if (this.closestBoat?.arrived) {
            //console.log(t, timeArrived, (t - timeArrived) / t * 100);
        }
    }
    compute(arrivedRate = 0.05) {
        return new Promise((resolve, reject) => {
            var compute = () => {
                for (var i = 0; i < 5000 && this.arrived / this.boats.length < arrivedRate; i++) {
                    this.act();
                }
                if (this.arrived / this.boats.length < arrivedRate) {
                    setImmediate(compute);
                } else {
                    resolve();
                }
            }
            compute();
        });
    }
}
module.exports=Simulation;