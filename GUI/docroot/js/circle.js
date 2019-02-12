const Circle = new Class({ 
   Implements: [Options, Events],

   initialize: function (options) {
      this.setOptions(options);

      this.draw().attach();
   },

   attach: function () {
      this.addEvent('change', this.draw);
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

      // pickstrategy_context.clearRect(this.options.x, this.options.y, this.options.radius * 2, this.options.radius * 2); 
      context.beginPath();
      context.arc(this.options.x, this.options.y, this.options.radius, 0, 2 * Math.PI);
      context.fillStyle = this.options.background;
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