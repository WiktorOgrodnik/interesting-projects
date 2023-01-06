use std::fs::File;
use core::result::Result;
use std::io::Write;

pub enum ImageType {
	Bmp,
	Ppm,
}

trait ImageImplementation {
	fn render(&self, width: usize, height: usize, data: Vec<u32>) -> Vec<u8>;
	fn get_extension(&self) -> String;
}

struct ImagePPM;

impl ImageImplementation for ImagePPM {
	fn render(&self, width: usize, height: usize, data: Vec<u32>) -> Vec<u8> {
		let mut header: String = format!("P3\n# fractal.ppm\n{} {}\n255\n", width, height);
			
		let strings = data.iter().map(|&e| format!("{} {} {}\n", (e >> 16) & 0xFF, (e >> 8) & 0xFF, e & 0xFF)).collect::<Vec<String>>().join("");

		header.push_str(&format!("{}", strings));
		header.as_bytes().to_vec()
	}

	fn get_extension(&self) -> String {
		String::from("ppm")
	}
}

struct ImageBMP;

impl ImageImplementation for ImageBMP {
	fn render(&self, width: usize, height: usize, data: Vec<u32>) -> Vec<u8> {

		let bmp_padding: [u8; 3] = [0, 0, 0];
		let padding_ammount = (4 - (width * 3) % 4) % 4;

		const FILE_HEADER_SIZE: usize = 14;
		const FILE_INFORMATION_SIZE: usize = 40;
		let file_data_size = width * height * 3 + padding_ammount * height;

		let file_size = FILE_HEADER_SIZE + FILE_INFORMATION_SIZE + file_data_size;
		let mut file_data: Vec<u8> = vec![
			'B' as u8,
			'M' as u8,
			file_size as u8,
			(file_size >> 8) as u8,
			(file_size >> 16) as u8,
			(file_size >> 24) as u8,
			0, 0, 0, 0,
			(FILE_HEADER_SIZE + FILE_INFORMATION_SIZE) as u8,
			0, 0, 0,
			FILE_INFORMATION_SIZE as u8,
			0, 0, 0,
			// image width
			width as u8,
			(width >> 8) as u8,
			(width >> 16) as u8,
			(width >> 24) as u8,
			// image height
			height as u8,
			(height >> 8) as u8,
			(height >> 16) as u8,
			(height >> 24) as u8,
			// planes
			1, 0,
			// Bits per pixel (RGB)
			24,
		];
			
		// Other unused
		for _ in 15..40 { file_data.push(0); }
			
		for y in 0..height {
			for x in 0..width {
				let i = data[(height - y - 1) * width + x];
				let s = [i as u8, (i >> 8) as u8, (i >> 16) as u8];
				file_data.extend_from_slice(&s);
			}
			file_data.extend_from_slice(&bmp_padding[0..padding_ammount])
		}

		file_data
	}

	fn get_extension(&self) -> String {
		String::from("bmp")
	}
}

pub struct Image {
	width: usize,
	height: usize,
	data: Vec<u32>,
	implementation: Box<dyn ImageImplementation>,
}

impl Image {

	pub fn new(width: usize, height: usize, itype: ImageType) -> Self {
		Image {
			width,
			height,
			data: vec![0 ; width * height],
			implementation: match itype {
				ImageType::Bmp => Box::new(ImageBMP),
				ImageType::Ppm => Box::new(ImagePPM),
			},
		}
	}
	
	pub fn set_pixel(&mut self, x: usize, y: usize, rgb: (u8, u8, u8)) {
		self.data[y * self.width + x] = (rgb.0 as u32) << 16 |
										(rgb.1 as u32) << 8 |
										(rgb.2 as u32);
	}
	
	pub fn save_image(self) -> Result<(), String> {
		match File::create(format!("fractal.{}", self.implementation.get_extension())) {
			Ok(mut file) => {
				match file.write_all(&self.implementation.render(self.width, self.height, self.data)[..]) {
					Ok(()) => Ok(()),
					Err(_) => Err("Failed to save file".to_string())
				}
			},
			Err(_) => Err(String::from("Failed to create a file"))
		}
	}
}
