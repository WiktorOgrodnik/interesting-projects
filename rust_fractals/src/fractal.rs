use crate::complex::*;
use crate::image::*;

pub enum FractalTypes {
	MandelbrotFractal,
	JuliaFractal(Complex),
	BurningShipFractal,
}

trait FractalImplementation {
	fn init(&self, point: Complex) -> Complex;
	fn cont(&self, prev: Complex, point: Complex) -> Complex;
}

pub struct MandelbrotFractal;

impl FractalImplementation for MandelbrotFractal {
	fn init(&self, _: Complex) -> Complex {
		Complex { re: 0.0, im: 0.0 }
	}

	fn cont(&self, prev: Complex, point: Complex) -> Complex {
		prev * prev + point
	}
}

pub struct JuliaFractal {
	constant: Complex,
}

impl JuliaFractal {
	fn new(constant: Complex) -> JuliaFractal {
		JuliaFractal {
			constant,
		}
	}
}

impl FractalImplementation for JuliaFractal {
	fn init(&self, point: Complex) -> Complex {
		point
	}

	fn cont(&self, prev: Complex, _: Complex) -> Complex {
		prev * prev + self.constant
	}
}

pub struct BurningShipFractal;

impl FractalImplementation for BurningShipFractal {
	fn init(&self, _: Complex) -> Complex {
		Complex { re: 0.0, im: 0.0 }
	}

	fn cont(&self, prev: Complex, point: Complex) -> Complex {
		let t = Complex { re: prev.re.abs(), im: prev.im.abs() };
		t * t + point
	}
}

pub struct Fractal {
	width_size: usize,
	height_size: usize,
	counting_points: Vec<Complex>,
	iterations: Vec<u8>,
	fractal_implementation: Box<dyn FractalImplementation>,
}

fn hsl(dist: u8) -> (u8, u8, u8) {
	(dist, ((dist as u32 * 5) % 255) as u8, ((dist as u32 * 12) % 255) as u8)
}
	
impl Fractal {
	
	pub fn new(width_size: usize, height_size: usize, fractal_type: FractalTypes) -> Self {
		Fractal {
			width_size,
			height_size,
			counting_points: vec![Complex { re: 0.0, im: 0.0, }; width_size * height_size],
			iterations: vec![0 ; width_size * height_size],
			fractal_implementation: match fractal_type {
				FractalTypes::MandelbrotFractal => Box::new(MandelbrotFractal),
				FractalTypes::JuliaFractal(constant) => Box::new(JuliaFractal::new(constant)),
				FractalTypes::BurningShipFractal => Box::new(BurningShipFractal),
			}
		}
	}
	
	fn calculate_for_point(&mut self, idx: usize, point: Complex) {
		let n = 200;

		self.counting_points[idx] = self.fractal_implementation.init(point);
	
		for i in 0..n {
			self.counting_points[idx] = self.fractal_implementation.cont(self.counting_points[idx], point);
			if self.counting_points[idx].module_square() >= 4.0 {
				self.iterations[idx] = i;
				break;
			}
		}
	}
	
	pub fn calculate(&mut self, zoom: f64, actual_cordinates: (f64, f64)) {
		let center = (self.width_size / 2, self.height_size / 2);
		let width_resolution = (2.0 / (self.width_size as f64 / 2.0)) / zoom;
		let height_resolution = (2.0 / (self.height_size as f64 / 2.0)) / zoom;
	
		for y in 0..self.width_size {
			for x in 0..self.height_size {
				let point = y * self.width_size + x;
				let c = Complex::new(actual_cordinates.0 + (x as i64 - center.0 as i64) as f64 * width_resolution,
												  actual_cordinates.1 + (y as i64 - center.1 as i64) as f64 * height_resolution);
				if c.module_square() <= 4.0 {
					self.calculate_for_point(point, c);
				} else {
					self.counting_points[point].re = f64::NAN;
					self.counting_points[point].im = f64::NAN;
				}
			}
		}
	}

	pub fn save_to_image(self, image: &mut Image) {

		for y in 0..self.width_size {
			for x in 0..self.height_size {
				let point = y * self.width_size + x;
				if self.counting_points[point].module_square() <= 4.0 {
					image.set_pixel(x, y, (0, 0, 0));
				} else {
					let dist = self.iterations[point];
					image.set_pixel(x, y, hsl(dist));
				}
			}
		}
	}
}

#[cfg(test)]
mod tests {
	use super::*;

	#[test]
	fn time_calculation() {
		const WIDTH: usize = 4096;
		const HEIGHT: usize = 4096;

		let mut fractal = Fractal::new(WIDTH, HEIGHT, FractalTypes::MandelbrotFractal);
		fractal.calculate(2.5, (-0.6, -0.7));

		assert!(true);
	}
}