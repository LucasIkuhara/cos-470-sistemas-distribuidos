use std::env;
use sysinfo::{Pid, SystemExt, ProcessExt};


fn main() {

    // Get all args from argv
    let mut args =  env::args_os();
    
    // Discard first argument, which is the executable path
    args.next().unwrap();

    // Parse PID and SIGNAL
    let pid: usize = args.next()
        .expect("Missing PID argument. (run as ./signaler <pid> <signal>)")
        .into_string().unwrap()
        .parse().expect("The received value for PID cannot be cast to int. Please use numbers only.");

    let signal: i32 = args.next()
        .expect("Missing SIGNAL argument. (run as ./signaler <pid> <signal>)")
        .into_string().unwrap()
        .parse().expect("The received value for SIGNAL cannot be cast to int. Please use numbers only.");

    println!("PID: {}", pid);
    println!("SIGNAL: {}", signal);

    // Get and update system info
    let mut system = sysinfo::System::new_all();
    system.refresh_all();
    let pid: Pid = Pid::from(pid);

    let proc = system.process(pid).expect("Chosen PID does not exist.");
    println!("PID {} exists and is {}", pid, proc.status());
    
}
