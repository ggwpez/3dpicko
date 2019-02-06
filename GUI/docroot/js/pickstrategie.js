var canvas,
    context;

const Circle = new Class({
  
  Implements: [Options, Events],
  
  initialize: function (options) {
    this.setOptions(options);
    
    this.draw().attach();
  },
  
  attach: function () {
    this.addEvent('change', this.draw);
  },
  
  set: function (what, value) {
  	const is_new = (this.options[what] != value);
    
    if (is_new)
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
   // context.clearRect(this.options.x, this.options.y, this.options.radius * 2, this.options.radius * 2); 
    
    context.beginPath();
		context.arc(this.options.x + this.options.radius, this.options.y + this.options.radius, this.options.radius, 0, 2 * Math.PI, false);
		context.fillStyle = this.options.background;
		context.fill();
		context.lineWidth = 3;
		context.strokeStyle = this.options.linecolor;
      	context.stroke();
  
    return this;
  },
  
  getSize: function () {
    return {
			width: this.options.radius * 2,
		  height: this.options.radius * 2
		}
  },
  
  getPosition: function () {
    var position = {
			x: this.get('x'),
			y: this.get('y')
		}
    
		return position;
  },
  
  isMouseOver: function (x, y) {
    var position = this.getPosition(),
				size   = this.getSize(),
				radius = this.options.radius,
				centerX = position.x + (size.width / 2),
				centerY = position.y + (size.height / 2),
				distanceX = x - centerX,
				distanceY = y - centerY;
    
    var d = Math.round(Math.sqrt(Math.pow(x - centerX, 2) + Math.pow(y - centerY, 2)));
			
		return d <= radius;
  }
  
});

var balls;
var collided_id, old_collided_id;
function drawWells()
{
	collided_id = -1, old_collided_id = -1;
	balls = [];
	canvas  = document.getElementById('circle'),
	context = canvas.getContext('2d');

	for (var x = 0; x < 24; ++x)
	for (var y = 0; y < 16; ++y)
	{
		ball = new Circle({
			x: x *32 +100,
			y: y *32 +100,
			radius: 14,
			background: 'white'
		});

		ball.addEvent('mouseenter', function () {
			this.set('background', 'green');
		});

		ball.addEvent('mouseleave', function () {
			this.set('background', 'white');
		});

		balls.push(ball);
	}
	context.font = "16px Arial";
	context.fillStyle = "red";
	for (var i = 0; i < 24; ++i)
		context.fillText(i, 100 +8 +32 *i, 66);
	for (var i = 0; i < 16; ++i)
		context.fillText(String.fromCharCode(65 +i), 50, 32 *i +120);

	canvas.addEvent('mousemove', function (e)
	{
		var rect = canvas.getBoundingClientRect();
		var x = e.client.x -rect.left,
		y = e.client.y -rect.top;

		for (var i = 0; i < balls.length; ++i)
		{
			var ball = balls[i];

			if (ball.isMouseOver(x, y))
			{
				if (! ball.mouseover)
				{
					ball.fireEvent('mouseenter');
					ball.mouseover = true;
					collided_id = i;
				}
			}
			else if (ball.mouseover)
			{
				ball.mouseover = false;
				ball.fireEvent('mouseleave');
			}
		}

		if (old_collided_id != collided_id)
		{
			for (var i = 0; i < balls.length; ++i)
			{
				balls[i].set('background', (i < collided_id ? 'white' : 'green'));
			}
			old_collided_id = collided_id;
		}
	});
}