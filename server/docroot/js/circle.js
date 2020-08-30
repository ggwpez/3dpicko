/*
 * SPDX-FileCopyrightText: 2018-2020 Harald Gültig <hgueltig@gmail.com>
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

// TODO subclasses colony and well
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
        context.lineWidth = line_width || 2;
        context.strokeStyle = this.options.linecolor;
        if(!this.options.excluded_by || this.options.excluded_by == ""){
            context.arc(this.options.x, this.options.y, this.options.radius, 0, 2 * Math.PI);
            if(this.options.background){
                context.fillStyle = this.options.background;
                context.fill();
            }
            context.stroke();
        }
        else{
            // cross out colony
            context.lineWidth = line_width || 1;
            // 0.71 = sin(45deg)...
            let offset = 0.71 * this.options.radius;
            context.moveTo(this.options.x+offset, this.options.y+offset);
            context.lineTo(this.options.x-offset, this.options.y-offset);
            context.moveTo(this.options.x-offset, this.options.y+offset);
            context.lineTo(this.options.x+offset, this.options.y-offset);
            context.stroke();
        }

        return this;
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