class Vector {
    constructor(magnitude = 1, direction = 0, options = { cartesianOnly: false, polarOnly: false }) {
        this.Direction = direction;
        this.Magnitude = magnitude;
        this.polChanges = 0;
        this.carChanges = 0;
        this.options = options;
        this.X = Math.cos(this.Direction) * this.Magnitude;
        this.Y = Math.sin(this.Direction) * this.Magnitude;
    }
    set direction(direction) {
        this.Direction = direction;
        if (this.options.polarOnly) return;
        this.X = Math.cos(this.Direction) * this.Magnitude;
        this.Y = Math.sin(this.Direction) * this.Magnitude;
        this.polChanges++;
        if (this.polChanges > 10 && this.carChanges == 0) {
            throw ("Pol only");
        }
    }
    set magnitude(magnitude) {
        this.Magnitude = magnitude;
        if (this.options.polarOnly) return;
        this.X = Math.cos(this.Direction) * this.Magnitude;
        this.Y = Math.sin(this.Direction) * this.Magnitude;
    }
    set x(x) {
        this.X = x;
        this.carChanges++;
        if (this.options.cartesianOnly) return;
        if (this.carChanges > 10 && this.polChanges == 0) {
            var err = new Error();
            console.log(err.stack);
            throw ("car only");
        }
        this.Direction = Math.atan2(this.Y, this.X);
        this.Magnitude = Math.sqrt(Math.pow(this.X, 2) + Math.pow(this.Y, 2));
    }
    set y(y) {
        this.Y = y;
        if (this.options.cartesianOnly) return;
        this.Direction = Math.atan2(this.Y, this.X);
        this.Magnitude = Math.sqrt(Math.pow(this.X, 2) + Math.pow(this.Y, 2));
    }
    get direction() {
        return this.Direction;
    }
    get magnitude() {
        return this.Magnitude;
    }
    get x() {
        return this.X;
    }
    get y() {
        return this.Y;
    }
}
class CartesianVector {
    constructor(x = 0, y = 0) {
        this.x = x;
        this.y = y;
    }
    get magnitude() {
        return Math.sqrt(Math.pow(this.x, 2) + Math.pow(this.y, 2));
    }
    get direction(){
        return Math.atan2(this.y, this.x);
    }
}
class PolarVector {
    constructor(direction = 0, magnitude = 0) {
        this.direction = direction;
        this.magnitude = magnitude;
    }
}
exports.Vector=Vector;
exports.CartesianVector=CartesianVector;
exports.PolarVector=PolarVector;