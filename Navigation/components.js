class Component {
    constructor(data) {
        this.mass = data.mass || 0;
        this.inputPower = data.inputPower || 0;
        this.outputPower = data.outputPower || 0;
        this.priority = data.priority || 0;
        this.thrust = data.thrust || 0;
    }
}
class Motor extends Component {
    constructor(type = "pod-drive-1.0-evo") {
        var data = {
            "pod-drive-1.0-evo": {
                mass: 6200,
                inputPower: 1000,
                priority: 0,
                thrust: 71
            }
        }
        super(data[type] || data["pod-drive-1.0-evo"]);
    }
}
class Battery extends Component {
    constructor(type = "vertex-s-trina-solar") {
        var data = {
            "vertex-s-trina-solar": {
                mass: 21000,
                outputPower: 350
            }
        }
        super(data[type] || data["vertex-s-trina-solar"]);
    }
}

module.exports={Component:Component,Motor:Motor,Battery:Battery}