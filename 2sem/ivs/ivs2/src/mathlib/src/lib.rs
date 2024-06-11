//! This library provides functionality for recursively evaluating mathematical expressions.
//! It includes an enum `MathExpr` able to represent any mathematical expression.
//! The MathExpr enum includes variants
//! for many mathematical operations such as addition, multiplication,
//! exponentiation, factorial, and others. The `Number` variant holds a value of the type
//! `Decimal` from the [rust_decimal](https://crates.io/crates/rust_decimal) crate,
//! which allow for precise decimal arithmetic. This is done for the reason of eliminating
//! rounding errors, which are present when working with regular `f32` and `f64` types.
//!
//! The `MathExpr` type provides a method called `eval` to recursively evaluate a given
//! mathematical expression.
//!
//! # Example
//!
//! ```rust
//! # use rust_decimal_macros::dec;
//! use mathlib::MathExpr::{AddExpr, MulExpr, Number};
//!
//! let two = Box::new(Number(dec!(2)));
//! let three = Box::new(Number(dec!(3)));
//! let four = Box::new(Number(dec!(4)));
//!
//! // (2 + 3) * 4
//! let expr = MulExpr(
//!     Box::new(AddExpr(two, three)),
//!     four
//! );
//! assert_eq!(expr.eval(), Some(dec!(20)));
//! ```

use rust_decimal::prelude::*;
use rust_decimal_macros::dec;

/// Enum representing any mathematical expression.
///
/// Most operands must be wrapped in `Box<T>`,
/// because self-containing structures cannot be stored on the stack.
#[derive(Debug, Clone)]
pub enum MathExpr {
    /// Adds two operands.
    AddExpr(Box<MathExpr>, Box<MathExpr>),
    /// Subtracts second operand from first one.
    SubExpr(Box<MathExpr>, Box<MathExpr>),
    /// Multiplies two operands.
    MulExpr(Box<MathExpr>, Box<MathExpr>),
    /// Calculates the remainder of dividing first operand by second.
    ModExpr(Box<MathExpr>, Box<MathExpr>),
    /// Adds all operands together.
    ///
    /// This variant exists for optimization purposes, to avoid huge expression trees
    /// when constructing long sums.
    AddExprMultiple(Vec<MathExpr>),
    /// Multiples all operands together.
    ///
    /// Like `AddExprMultiple`, this exists for optimization.
    MulExprMultiple(Vec<MathExpr>),
    /// Divides first operand by second.
    DivExpr(Box<MathExpr>, Box<MathExpr>),
    /// Raises first operand to the power of second operand.
    PowExpr(Box<MathExpr>, Box<MathExpr>),
    /// Calculates n-th root of second operand, where n is given by the first operand.
    RootExpr(Box<MathExpr>, Box<MathExpr>),
    /// Calculates factorial of operand. Works only for non-negative whole numbers.
    FactExpr(Box<MathExpr>),
    /// Calculates the natural log of operand.
    LnExpr(Box<MathExpr>),
    /// Calculates absolute value of operand.
    AbsExpr(Box<MathExpr>),
    /// Calculates the square root of operand, faster than `RootExpr`.
    SqrtExpr(Box<MathExpr>),
    /// Inverts the sign of operand.
    NegExpr(Box<MathExpr>),
    /// This variant represents a constant value, of the type `Decimal`.
    ///
    /// More info about the `Decimal` type can be found at
    /// [`rust_decimal` documentation](https://docs.rs/rust_decimal/latest/rust_decimal/)
    Number(Decimal),
}

impl MathExpr {
    /// Recursively evaluates given expression and returns `Some` of value
    /// or `None`, if expression is invalid (such as division by zero, log of negative
    /// value, arithmetic overflow, etc).
    pub fn eval(&self) -> Option<Decimal> {
        match self {
            MathExpr::AddExpr(a, b) => {
                let (a, b) = (a.eval(), b.eval());
                if let (Some(a), Some(b)) = (a, b) {
                    a.checked_add(b)
                } else {
                    None
                }
            }

            MathExpr::SubExpr(a, b) => {
                let (a, b) = (a.eval(), b.eval());
                if let (Some(a), Some(b)) = (a, b) {
                    a.checked_sub(b)
                } else {
                    None
                }
            }

            MathExpr::MulExpr(a, b) => {
                let (a, b) = (a.eval(), b.eval());
                if let (Some(a), Some(b)) = (a, b) {
                    a.checked_mul(b)
                } else {
                    None
                }
            }

            MathExpr::ModExpr(a, b) => {
                let (a, b) = (a.eval(), b.eval());
                if let (Some(a), Some(b)) = (a, b) {
                    a.checked_rem(b)
                } else {
                    None
                }
            }

            MathExpr::AddExprMultiple(terms) => terms
                .iter()
                .map(|term| term.eval())
                .reduce(|a, b| {
                    if let (Some(a), Some(b)) = (a, b) {
                        a.checked_add(b)
                    } else {
                        None
                    }
                })
                .flatten(),

            MathExpr::MulExprMultiple(terms) => terms
                .iter()
                .map(|term| term.eval())
                .reduce(|a, b| {
                    if let (Some(a), Some(b)) = (a, b) {
                        a.checked_mul(b)
                    } else {
                        None
                    }
                })
                .flatten(),

            MathExpr::DivExpr(a, b) => {
                let (a, b) = (a.eval(), b.eval());
                if let (Some(a), Some(b)) = (a, b) {
                    a.checked_div(b)
                } else {
                    None
                }
            }

            MathExpr::PowExpr(a, b) => {
                let (a, b) = (a.eval(), b.eval());
                if let (Some(a), Some(b)) = (a, b) {
                    let (a, b) = (a.to_f64(), b.to_f64());
                    if let (Some(a), Some(b)) = (a, b) {
                        Decimal::from_f64_retain(a.powf(b))
                    } else {
                        None
                    }
                } else {
                    None
                }
            }

            MathExpr::RootExpr(a, b) => {
                let (a, b) = (a.eval(), b.eval());
                if let (Some(a), Some(b)) = (a, b) {
                    let (a, b) = (a.to_f64(), b.to_f64());
                    if let (Some(a), Some(b)) = (a, b) {
                        Decimal::from_f64_retain(b.powf(1f64 / a))
                    } else {
                        None
                    }
                } else {
                    None
                }
            }

            MathExpr::FactExpr(a) => {
                if let Some(a) = a.eval() {
                    if a != a.round() || a.is_sign_negative() {
                        return None;
                    }

                    let mut counter = a;
                    let mut result = Some(dec!(1));

                    while counter > dec!(1) {
                        if let Some(r) = result {
                            result = r.checked_mul(counter);
                        }
                        counter -= dec!(1);
                    }
                    result
                } else {
                    None
                }
            }

            MathExpr::LnExpr(a) => {
                if let Some(a) = a.eval() {
                    a.checked_ln()
                } else {
                    None
                }
            }

            MathExpr::AbsExpr(a) => {
                if let Some(a) = a.eval() {
                    Some(a.abs())
                } else {
                    None
                }
            }

            MathExpr::SqrtExpr(a) => {
                if let Some(a) = a.eval() {
                    a.sqrt()
                } else {
                    None
                }
            }

            MathExpr::NegExpr(a) => {
                if let Some(a) = a.eval() {
                    a.checked_mul(dec!(-1))
                } else {
                    None
                }
            }

            MathExpr::Number(a) => Some(*a),
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use MathExpr::*;

    fn number(value: &str) -> MathExpr {
        Number(Decimal::from_str(value).unwrap())
    }

    fn box_number(value: &str) -> Box<MathExpr> {
        Box::new(Number(Decimal::from_str(value).unwrap()))
    }

    #[test]
    fn one_plus_one() {
        let a = box_number("1");
        let b = box_number("1");
        let c = AddExpr(a, b);
        assert_eq!(c.eval(), Some(dec!(2)));
    }

    #[test]
    fn one_plus_one_multiple() {
        let a = number("1");
        let b = number("1");
        let c = AddExprMultiple(vec![a, b]);
        assert_eq!(c.eval(), Some(dec!(2)));
    }

    #[test]
    fn addition_subtraction() {
        // (14 + (-77004 - 42000 + 78.58) - 0.666) == -118912.086
        let inner = AddExpr(
            Box::new(AddExpr(box_number("-77_004"), box_number("-42_000"))),
            box_number("78.58"),
        );
        let expr = AddExpr(
            Box::new(AddExpr(box_number("14"), Box::new(inner))),
            box_number("-0.666"),
        );
        assert_eq!(expr.eval(), Some(dec!(-118912.086)));
    }

    #[test]
    fn addition_subtraction_multiple() {
        // (14 + (-77004 - 42000 + 78.58) - 0.666) == -118912.086
        let inner = AddExprMultiple(vec![number("-77_004"), number("-42_000"), number("78.58")]);
        let expr = AddExprMultiple(vec![number("14"), inner, number("-0.666")]);
        assert_eq!(expr.eval(), Some(dec!(-118912.086)));
    }

    #[test]
    fn multiplication() {
        // (-4.35 * (0.44 * 80) * -78.5 * -0.5) == 6009.96
        let inner = MulExpr(box_number("0.44"), box_number("80"));
        let expr = MulExprMultiple(vec![number("4.35"), inner, number("-78.5"), number("-0.5")]);
        assert_eq!(expr.eval(), Some(dec!(6009.96)));
    }

    #[test]
    fn multiplication_multiple() {
        // (-4.35 * (0.44 * 80) * -78.5 * -0.5) == 6009.96
        let inner = MulExprMultiple(vec![number("0.44"), number("80")]);
        let expr = MulExprMultiple(vec![number("4.35"), inner, number("-78.5"), number("-0.5")]);
        assert_eq!(expr.eval(), Some(dec!(6009.96)));
    }

    #[test]
    fn division() {
        // -4466.7 / ( 97 / -7.3) == 336.1537 (4 decimal places)
        let divisor = DivExpr(box_number("97"), box_number("-7.3"));
        let expr = DivExpr(box_number("-4466.7"), Box::new(divisor));
        let value = expr.eval().unwrap();
        assert_eq!(format!("{value:.4}"), "336.1537");
    }

    #[test]
    fn division_by_zero() {
        // -4466.7 / ( 0 / -7.3) == None
        let divisor = DivExpr(box_number("0"), box_number("-7.3"));
        assert_eq!(divisor.eval(), Some(dec!(0)));
        let expr = DivExpr(box_number("-4466.7"), Box::new(divisor));
        assert_eq!(expr.eval(), None);
    }

    #[test]
    fn square_root() {
        // 4 ** (3 ** 2) == 262144
        let expr = SqrtExpr(box_number("4"));
        assert_eq!(expr.eval(), Some(dec!(2)));

        let expr = SqrtExpr(box_number("-4"));
        assert_eq!(expr.eval(), None);
    }

    #[test]
    fn exponentiation_integers() {
        // 4 ** (3 ** 2) == 262144
        let exp = PowExpr(box_number("3"), box_number("2"));
        let expr = PowExpr(box_number("4"), Box::new(exp));
        assert_eq!(expr.eval(), Some(dec!(262144)));
    }

    #[test]
    fn exponentiation_decimals() {
        // 3.5 ** (7.8 ** -1.4) == 1.07317 (5 decimal places)
        let exp = PowExpr(box_number("7.8"), box_number("-1.4"));
        let expr = PowExpr(box_number("3.5"), Box::new(exp));
        let value = expr.eval().unwrap();
        assert_eq!(format!("{value:.5}"), "1.07317");

        let expr = PowExpr(
            box_number("9"),
            Box::new(DivExpr(box_number("1"), box_number("2"))),
        );
        assert_eq!(expr.eval().unwrap(), dec!(3));
    }

    #[test]
    fn root_expr() {
        let expr = RootExpr(box_number("3"), box_number("8"));
        assert_eq!(expr.eval().unwrap(), dec!(2));
        let expr = RootExpr(box_number("0"), box_number("8"));
        assert!(expr.eval().is_none());
        let expr = RootExpr(box_number("-3"), box_number("8"));
        assert_eq!(expr.eval().unwrap(), dec!(0.5));
        let expr = RootExpr(box_number("1"), box_number("8"));
        assert_eq!(expr.eval().unwrap(), dec!(8));
    }

    #[test]
    fn exponentiation_invalid() {
        // -1 ** 0.5 == None
        let expr = PowExpr(box_number("-1"), box_number("0.5"));
        assert_eq!(expr.eval(), None);
    }

    #[test]
    fn modulo() {
        let expr = ModExpr(box_number("28"), box_number("12"));
        assert_eq!(expr.eval(), Some(dec!(4)));
    }

    #[test]
    fn factorial() {
        let expr = FactExpr(box_number("7"));
        assert_eq!(expr.eval(), Some(dec!(5040)));
        let expr = FactExpr(box_number("1"));
        assert_eq!(expr.eval(), Some(dec!(1)));
        let expr = FactExpr(box_number("0"));
        assert_eq!(expr.eval(), Some(dec!(1)));
    }

    #[test]
    fn factorial_invalid() {
        let expr = FactExpr(box_number("73456"));
        assert_eq!(expr.eval(), None);
        let expr = FactExpr(box_number("-1"));
        assert_eq!(expr.eval(), None);
        let expr = FactExpr(box_number("7.5"));
        assert_eq!(expr.eval(), None);
    }

    #[test]
    fn natural_log() {
        let expr = LnExpr(box_number("3657.785"));
        assert_eq!(format!("{:.6?}", expr.eval().unwrap()), "8.204613");
    }

    #[test]
    fn natural_log_invalid() {
        let expr = LnExpr(box_number("0"));
        assert_eq!(expr.eval(), None);
        let expr = LnExpr(box_number("-1"));
        assert_eq!(expr.eval(), None);
        let expr = LnExpr(box_number("-897.5"));
        assert_eq!(expr.eval(), None);
        let expr = FactExpr(box_number("7.5"));
        assert_eq!(expr.eval(), None);
    }

    #[test]
    fn absolute_value() {
        let expr = AbsExpr(box_number("0"));
        assert_eq!(expr.eval(), Some(dec!(0)));
        let expr = AbsExpr(box_number("234.34"));
        assert_eq!(expr.eval(), Some(dec!(234.34)));
        let expr = AbsExpr(box_number("-897.5"));
        assert_eq!(expr.eval(), Some(dec!(897.5)));
    }

    #[test]
    fn minus_sign() {
        let expr = NegExpr(box_number("0"));
        assert_eq!(expr.eval(), Some(dec!(0)));
        let expr = NegExpr(box_number("234.34"));
        assert_eq!(expr.eval(), Some(dec!(-234.34)));
        let expr = NegExpr(box_number("-897.5"));
        assert_eq!(expr.eval(), Some(dec!(897.5)));
    }

    #[test]
    fn large_expression() {
        // -1 * (-998 + 45 ** (44.7 - 44.125)) /
        // (3.2 * 63.004 * (78.44 ** -8 - 34.996)) == 0.142712 (6 decimal digits)
        let first_subexpr = PowExpr(
            box_number("45"),
            Box::new(AddExprMultiple(vec![number("44.7"), number("-44.125")])),
        );
        let first_expr = MulExpr(
            box_number("-1"),
            Box::new(AddExpr(box_number("998"), Box::new(first_subexpr))),
        );

        let second_subexpr = AddExprMultiple(vec![
            number("-34.996"),
            PowExpr(box_number("78.44"), box_number("-8")),
        ]);
        let second_expr = MulExprMultiple(vec![number("3.2"), number("63.004"), second_subexpr]);

        let value = DivExpr(Box::new(first_expr), Box::new(second_expr))
            .eval()
            .unwrap();
        assert_eq!(format!("{value:.6?}"), "0.142712");
    }
}
