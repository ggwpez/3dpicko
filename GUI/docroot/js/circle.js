const Circle = new Class({ 
   Implements: [Options, Events],

   initialize: function (options) {
      this.setOptions(options);

      this.draw().attach();
   },

   attach: function () {
      this.addEvent('change', this.draw);
      this.addEvent('toggle-select', this.toggleSelect);
   },

   toggleSelect: function ()
   {
      if (this.options.selected == true)
      {
         this.options.selected = false;  
         this.set('linecolor', 'white');
      }
      else
      {
         this.options.selected = true;
         this.set('linecolor', 'red');
      }

      console.log("Selected: ", this.options.selected);
   },

   // This function implies that a null value is as good as a nonexistent one.
   set: function (what, value) {
      if (this.options[what] != value)
      {
         this.options[what] = value;
         this.fireEvent('change');
      }
      return this;
   },

   get: function (what) {
      return this.options[what];
   },

   draw: function () {
      var canvas = this.options.canvas;
      var context = canvas.getContext('2d');

      context.beginPath();
      context.fillStyle = this.options.background;
      context.arc(this.options.x, this.options.y, this.options.radius, 0, 2 * Math.PI);
      context.fill();

      context.lineWidth = 3;
      context.strokeStyle = this.options.linecolor;
      context.stroke();

      return this;
   },

   getPosition: function () {
      var position = {
      x: this.options.x,
      y: this.options.y
      }

      return position;
   },

   isMouseOver: function (x, y)
   {
      var distanceX = x - this.options.x;
      var distanceY = y - this.options.y;

      var d = Math.sqrt(Math.pow(distanceX, 2) + Math.pow(distanceY, 2));

      return (d <= this.options.radius);
   }
});