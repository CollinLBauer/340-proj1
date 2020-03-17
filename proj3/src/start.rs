use std::env;
use std::thread;

fn main() {

    let args: Vec<String> = env::args().collect();
    let num_threads = (&args[1]).parse::<i32>().unwrap();

    // Make a vector to hold the children which are spawned.
    let mut children = vec![];

    for i in 0..num_threads {
        // Spin up another thread
        children.push(thread::spawn(move || {
            println!("this is thread number {}", i);
        }));
    }

    for child in children {
        // Wait for the thread to finish. Returns a result.
        let _ = child.join();
    }
}