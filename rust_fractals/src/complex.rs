use std::ops;

#[derive(Clone, Copy, Debug)]
pub struct Complex {
	pub re: f64,
	pub im: f64,
}

impl Complex {
	pub fn new(re: f64, im: f64) -> Self {
		Complex { re, im }
	}
}

impl Complex {
	pub fn module(self) -> f64 {
		(self.re * self.re + self.im * self.im).sqrt()
	}

	pub fn module_square(self) -> f64 {
		(self.re * self.re + self.im * self.im).sqrt()
	}
}

impl ops::Add<Complex> for Complex {
	type Output = Complex;

	fn add(self, arg: Complex) -> Complex {
		Complex {
			re: self.re + arg.re,
			im: self.im + arg.im,
		}
	}
}

impl ops::Sub<Complex> for Complex {
	type Output = Complex;

	fn sub(self, arg: Complex) -> Complex {
		Complex {
			re: self.re - arg.re,
			im: self.im - arg.im,
		}
	}
}

impl ops::Mul<Complex> for Complex {
	type Output = Complex;

	fn mul(self, arg: Complex) -> Complex {
		Complex {
			re: self.re * arg.re - self.im * arg.im,
			im: self.im * arg.re + self.re * arg.im,
		}
	}
}
