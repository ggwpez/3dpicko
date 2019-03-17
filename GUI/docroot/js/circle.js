class Circle{
    constructor(options){
        this.options = options;
    }

    exclude(){
        this.options.excluded_by = "user";
        delete this.options.included_by;
        this.set('linecolor', 'red');
        return this;
    }

    include(){
        this.options.excluded_by = "";
        this.options.included_by = "user";
        this.set('linecolor', 'white');
        return this;
    }

    toggleSelect(){
        if (this.options.selected == true){
            this.options.selected = false;
            this.set('linecolor', this.options.defaultLinecolor);
        }
        else{
            this.options.selected = true;
            this.set('linecolor', 'red');
        }
        console.log("Selected: ", this.options.selected);
    }

    // This function implies that a null value is as good as a nonexistent one.
    set(what, value){
        if (value == null){
            console.error("Cant set option ", what, " to null");
            return;
        }
        if (this.options[what] != value){
            this.options[what] = value;
            this.draw();
        }
        return this;
    }

    get(what){
        return this.options[what];
    }

    draw(ctx, line_width){
        let context = ctx || this.options.canvas.getContext('2d');

        context.beginPath();
        context.arc(this.options.x, this.options.y, this.options.radius, 0, 2 * Math.PI);
        if(this.options.background){
            context.fillStyle = this.options.background;
            context.fill();
        }

        context.lineWidth = line_width || 2;
        context.strokeStyle = this.options.linecolor;
        context.stroke();

        return this;
    }

    eraseBorder(){
        let context = this.options.canvas.getContext('2d');
        context.globalCompositeOperation = 'destination-out';
        context.beginPath();
        context.arc(this.options.x, this.options.y, this.options.radius, 0, 2 * Math.PI);
        context.lineWidth = 2.5;
        context.stroke();     
        context.globalCompositeOperation = 'source-over';   
    }

    getRadius(){
        return this.options.radius;
    }

    getPosition(){
        var position = {
            x: this.options.x,
            y: this.options.y
        }

        return position;
    }

    getColor(){
        return this.options.linecolor;
    }

    isMouseOver(x, y){
        // TODO review this
        var distanceX = x - this.options.x;
        var distanceY = y - this.options.y;

        var d = Math.sqrt(Math.pow(distanceX, 2) + Math.pow(distanceY, 2));

        return (d <= this.options.radius);
    }
}