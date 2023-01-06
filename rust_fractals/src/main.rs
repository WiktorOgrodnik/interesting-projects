pub mod fractal;
pub mod image;
pub mod complex;

use image::{Image, ImageType};
use fractal::Fractal;
use std::time::Instant;
use complex::Complex;

fn main() {

    const WIDTH: usize = 4096;
    const HEIGHT: usize = 4096;

    let mut fractal = Fractal::new(WIDTH, HEIGHT, fractal::FractalTypes::JuliaFractal(Complex { re: 0.45, im: -0.1}));

    let start = Instant::now();

    fractal.calculate(2.5, (-0.6, -0.7));

    let duration = start.elapsed();
    println!("Time passed: {}", duration.as_millis());

    let mut image = Image::new(WIDTH, HEIGHT, ImageType::Bmp);
    fractal.save_to_image(&mut image);
    image.save_image().unwrap();
}
