use mathlib::MathExpr::*;
use rust_decimal::Decimal;
use rust_decimal_macros::dec;
use std::io::{self, Read};
use std::str::FromStr;

fn stddev(numbers: &Vec<Decimal>) -> Option<Decimal> {
    let one = Box::new(Number(dec!(1)));
    let two = Box::new(Number(dec!(2)));

    // N
    let len = Box::new(Number(numbers.len().into()));

    // (1 / N) * sum x_i
    let x_bar = Box::new(DivExpr(
        Box::new(AddExprMultiple(
            numbers.iter().map(|xi| Number(*xi)).collect(),
        )),
        len.clone(),
    ))
    .eval()
    .unwrap();
    let x_bar = Box::new(Number(x_bar));

    // 1 / (N - 1)
    let first_fraction = Box::new(DivExpr(
        one.clone(),
        Box::new(SubExpr(len.clone(), one.clone())),
    ));

    let long_sum = Box::new(AddExprMultiple(
        numbers
            .iter()
            .map(|xi| {
                SubExpr(
                    Box::new(PowExpr(Box::new(Number(*xi)), two.clone())),
                    Box::new(MulExpr(
                        one.clone(), // len.clone(),
                        Box::new(PowExpr(x_bar.clone(), two.clone())),
                    )),
                )
            })
            .collect(),
    ));

    // the whole thing
    let stddev = PowExpr(
        Box::new(MulExpr(first_fraction, long_sum)),
        Box::new(Number(dec!(0.5))),
    );

    stddev.eval()
}

fn main() -> io::Result<()> {
    let mut numbers = String::new();
    io::stdin().read_to_string(&mut numbers)?;
    let numbers: Vec<Decimal> = numbers
        .split_whitespace()
        .map(|n| Decimal::from_str(n).unwrap())
        .collect();

    if let Some(s) = stddev(&numbers) {
        println!("{}", s);
    }
    Ok(())
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test1() {
        assert_eq!(
            stddev(&vec![dec!(1), dec!(2)]).unwrap().round_dp(5),
            dec!(0.70711)
        )
    }

    #[test]
    fn test2() {
        let input = vec![
            "-915",
            "545.35353",
            "-443.34533",
            "-0.00051",
            "54",
            "0",
            "4354",
            "433",
            "-1141.54545",
            "434",
            "765",
        ]
        .into_iter()
        .map(|n| Decimal::from_str(n).unwrap())
        .collect();
        assert_eq!(stddev(&input).unwrap().round_dp(2), dec!(1450.46))
    }
}
