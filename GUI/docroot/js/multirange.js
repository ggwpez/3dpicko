var multirange = function(input) {
    let descriptor = Object.getOwnPropertyDescriptor(HTMLInputElement.prototype, "value");
    let value = input.getAttribute("value");
    let values = value === null ? [] : value.split(",");
    let min = +(input.min || 0);
    let max = +(input.max || 100);
    let ghost = input.cloneNode();

    input.classList.add("multirange", "original");
    ghost.classList.add("multirange", "ghost");

    input.value = values[0] || min + (max - min) / 2;
    ghost.value = values[1] || min + (max - min) / 2;

    input.parentNode.insertBefore(ghost, input.nextSibling);

    Object.defineProperty(input, "originalValue", descriptor.get ? descriptor : {
        // Fuck you Safari >:(
        get: function() { return this.value; },
        set: function(v) { this.value = v; }
    });

    Object.defineProperties(input, {
        valueLow: {
            get: function() { return Math.min(this.originalValue, ghost.value); },
            set: function(v) { this.originalValue = v; update();},
            enumerable: true
        },
        valueHigh: {
            get: function() { return Math.max(this.originalValue, ghost.value); },
            set: function(v) { ghost.value = v; update();},
            enumerable: true
        }
    });

    if (descriptor.get) {
        // Again, fuck you Safari
        Object.defineProperty(input, "value", {
            get: function() { return this.valueLow + "," + this.valueHigh; },
            set: function(v) {
                let values = v.split(",");
                this.valueLow = values[0];
                this.valueHigh = values[1];
                update();
            },
            enumerable: true
        });
    }

    if (typeof input.oninput === "function") {
        ghost.oninput = input.oninput.bind(input);
    }

    function update() {
        ghost.style.setProperty("--low", 100 * ((input.valueLow - min) / (max - min)) + 1 + "%");
        ghost.style.setProperty("--high", 100 * ((input.valueHigh - min) / (max - min)) - 1 + "%");
    }

    ghost.addEventListener("mousedown", function(evt) {
        // is ghost lower in range
        let isInverted = input.valueLow == ghost.value;
        let clickValue = min + Math.abs(min-max) * evt.offsetX / this.offsetWidth;
        // get distance to high and low values in the range
        let highDiff = Math.abs(input.valueHigh - clickValue);
        let lowDiff = Math.abs(input.valueLow - clickValue);
        if (isInverted ? lowDiff > highDiff : lowDiff < highDiff) {
            // update "lower" slider 
            evt.preventDefault();
            if(isInverted) input.valueHigh = clickValue;
            else input.valueLow = clickValue;
            input.oninput();
            return false;
        }
    });

    input.addEventListener("input", update);
    ghost.addEventListener("input", update);
    update();
}

multirange.init = function() {
 [].slice.call(document.querySelectorAll("input[type=range][multiple]:not(.multirange)")).forEach(multirange);
}