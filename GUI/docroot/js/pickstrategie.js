var pickstrategy_canvas = document.getElementById('pickstrategy-canvas');
var pickstrategy_context;

var balls, cols, rows;
var collided_id, old_collided_id;
// starting at 1
var collided_row = 1, collided_column = 1;
var mouse_down = false;

function drawWells(col_arg, rows_arg, colonys = 1)
{
	// TODO default starting well
	cols = col_arg;
	rows = rows_arg;
	let size;
	if(cols >= 20) size = 26;
	else size = 32;
	pickstrategy_context = pickstrategy_canvas.getContext('2d');
	pickstrategy_context.clearRect(0, 0, pickstrategy_canvas.width, pickstrategy_canvas.height);
	collided_id = -1, old_collided_id = -1;
	balls = [];
	pickstrategy_context.canvas.width = cols*size+50;
	pickstrategy_context.canvas.height = rows*size+55+20;

	for (var x = 0; x < cols; ++x){
		for (var y = 0; y < rows; ++y)
		{
			ball = new Circle({
				x: x * size + 60,
				y: y * size + 65,
				radius: size/2-2,
				background: 'white',
				canvas: pickstrategy_canvas
			});

			ball.addEvent('mouseenter', function () {
				this.set('background', 'green');
			});

			ball.addEvent('mouseleave', function () {
				// this.set('background', 'white');
			});
			
			balls.push(ball);
		}
	}
	pickstrategy_context.font = "16px Arial";
	pickstrategy_context.fillStyle = "red";
	for (var i = 0; i < cols; ++i) {
		pickstrategy_context.fillText(i+1, 50 + size *i, 30);
	}
	for (var i = 0; i < rows; ++i) {
		pickstrategy_context.fillText(String.fromCharCode(65 +i), 10, size * i + 70);
	}

}

pickstrategy_canvas.addEvent('mousemove', function (e)
{
	if(mouse_down){
		var rect = pickstrategy_canvas.getBoundingClientRect();
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
			
			pickstrategy_context.clearRect(0, pickstrategy_context.canvas.height-20, pickstrategy_context.canvas.width, 20); 
			pickstrategy_context.fillStyle = "black";
			collided_row = collided_id%rows+1;
			collided_column = (collided_id-collided_id%rows)/rows+1;
			pickstrategy_context.fillText("Start at well: "+String.fromCharCode(64 + collided_row)+(collided_column),0, pickstrategy_context.canvas.height);
		}
	} else return false;
});
pickstrategy_canvas.addEvent('mousedown', function (e){
	mouse_down = true;
	// What a cheat!
	pickstrategy_canvas.fireEvent('mousemove', e);
});
pickstrategy_canvas.addEvent('mouseup', function (e){
	mouse_down = false;
});