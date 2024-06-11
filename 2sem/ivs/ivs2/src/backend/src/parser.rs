//! This module implements mathematical expression parsing using the Pratt parser generator
//! from the [pest](https://pest.rs/) crate. The grammar is stored in the
//! file `math.pest`, and the parser is generated once, during compilation.

use mathlib::MathExpr;
use MathExpr::*;

use pest::error;
use pest::iterators::Pairs;
use pest::pratt_parser::{Assoc, Op, PrattParser};
use pest::Parser;
use pest_derive::Parser;

use lazy_static::lazy_static;
use rust_decimal::prelude::*;

/// This is the parser struct, it is generated at compile-time by Pest.
#[derive(Parser)]
#[grammar = "math.pest"]
struct MathExprParser;

const INTERNAL_ERROR_MSG: &str = "Pepega screwed up the parser: ";

lazy_static! {
    /// This parser object is instatiated once at runtime, then it is used repeatedly.
    /// Operator precedence is defined here.
    static ref PARSER: PrattParser<Rule> = PrattParser::new()
        .op(Op::infix(Rule::addition_op, Assoc::Left)
            | Op::infix(Rule::subtraction_op, Assoc::Left))
        .op(Op::infix(Rule::multiplication_op, Assoc::Left)
            | Op::infix(Rule::division_op, Assoc::Left)
            | Op::infix(Rule::modulo_op, Assoc::Left))
        .op(Op::infix(Rule::power_op, Assoc::Left) | Op::infix(Rule::root_op, Assoc::Left))
        .op(Op::postfix(Rule::factorial_op))
        .op(Op::prefix(Rule::abs_op))
        .op(Op::prefix(Rule::ln_op))
        .op(Op::prefix(Rule::sqrt_op))
        .op(Op::prefix(Rule::minus_sign));
}

/// This function is called recursively to parse the expression. It takes an iterator
/// over tokens, and returns a `Result` of MathExpr, or a parser error.
fn parse_pairs(input_pairs: Pairs<Rule>) -> Result<MathExpr, pest::error::Error<Rule>> {
    PARSER
        .map_primary(|subexpr| match subexpr.as_rule() {
            Rule::number => {
                if let Ok(number) = Decimal::from_str(subexpr.as_str()) {
                    Ok(Number(number))
                } else {
                    Err(error::Error::new_from_span(
                        error::ErrorVariant::CustomError {
                            message: "Invalid number".to_string(),
                        },
                        subexpr.as_span(),
                    ))
                }
            }

            Rule::subexpr => parse_pairs(subexpr.into_inner()),
            // Rule => dbg!(Rule),
            rule => unreachable!("{}{:?}", INTERNAL_ERROR_MSG, rule),
        })
        .map_prefix(|op, subexpr| {
            if let Ok(subexpr) = subexpr {
                match op.as_rule() {
                    Rule::minus_sign => Ok(NegExpr(Box::new(subexpr))),
                    Rule::abs_op => Ok(AbsExpr(Box::new(subexpr))),
                    Rule::ln_op => Ok(LnExpr(Box::new(subexpr))),
                    Rule::sqrt_op => Ok(SqrtExpr(Box::new(subexpr))),
                    _ => unreachable!("{}", INTERNAL_ERROR_MSG),
                }
            } else {
                subexpr
            }
        })
        .map_postfix(|subexpr, op| {
            if let Ok(subexpr) = subexpr {
                match op.as_rule() {
                    Rule::factorial_op => Ok(FactExpr(Box::new(subexpr))),
                    rule => unreachable!("{}{:?}", INTERNAL_ERROR_MSG, rule),
                }
            } else {
                subexpr
            }
        })
        .map_infix(|left, op, right| {
            if left.is_err() {
                return left;
            }
            if right.is_err() {
                return right;
            }

            let left = left.unwrap();
            let right = right.unwrap();

            match op.as_rule() {
                Rule::addition_op => Ok(AddExpr(Box::new(left), Box::new(right))),
                Rule::subtraction_op => Ok(SubExpr(Box::new(left), Box::new(right))),
                Rule::multiplication_op => Ok(MulExpr(Box::new(left), Box::new(right))),
                Rule::division_op => Ok(DivExpr(Box::new(left), Box::new(right))),
                Rule::modulo_op => Ok(ModExpr(Box::new(left), Box::new(right))),
                Rule::power_op => Ok(PowExpr(Box::new(left), Box::new(right))),
                Rule::root_op => Ok(RootExpr(Box::new(left), Box::new(right))),
                _ => unreachable!("{}", INTERNAL_ERROR_MSG),
            }
        })
        .parse(input_pairs)
}

/// This function parses the whole input string into a math expression represented by
/// the [MathExpr] enum from the [mathlib] module.
pub fn parse_input(input: &str) -> Result<MathExpr, pest::error::Error<Rule>> {
    let input_pairs = MathExprParser::parse(Rule::mathexpr, input)?
        .next()
        .ok_or(error::Error::new_from_pos(
            error::ErrorVariant::CustomError {
                message: "Empty expression".to_string(),
            },
            pest::Position::from_start(""),
        ))?
        .into_inner();

    parse_pairs(input_pairs)
}

#[cfg(test)]
mod tests {
    use super::*;
    use rust_decimal_macros::dec;

    fn eval(input: &str) -> Decimal {
        parse_input(input).unwrap().eval().unwrap()
    }

    fn eval_err(input: &str) -> bool {
        parse_input(input).is_err()
    }

    #[test]
    fn invalid_inputs() {
        assert!(eval_err("@#€\\)!@#$#$^$&^^&* ḅ̷̛̛̳̮̖͓̃̓̿̔̓́͒̒̑̂͒͐̈́̓͋̈́̽́̾̋̈́̓̔͂͂̌̅̓͂̂̉̏̀̀̑̉̇́͑̓̊́̽̀͂̒̏̅͋̾̆̂̀͌̆̃͑̾̋̐̊͒͆̓̃̀̓̑̓̊͗͊̈́̾̿͂̂̔̈́͂̕͘̚̕͘̚͘̚̕̕̕͘͝͝r̸̨̡̡̢̨̢̨̢̨̡͖͇̖̟̦̖͉͔̜̣̲̮̹̫̭͕̠̯̤̹̹̹̫̹͎̪̙̤̗̺̠͎͓̰̺̰̗̙̩͈̖̬̲̜͎͉̘̮̘̥̩̠͙̳̖͎͚̣͍̺̟̜̯̫̹̺̞͔̱͎̈́̑̂͘͜ͅử̵̧̛̹͇̬͚͈̦̲̰̦̩͚͖̗̣̒́͗͗̅͐̈̔̑̓̔̿̿̎̔̃̈̈̔̊͝͝u̷̢̨̢̯͎̬̩̻̭͍̠̞̙̲̟̪̖̝̣̗͔̬̹̦̲̤̩͎̲͖̹͈͈͚͙̱͎̲͕͕͎͍̪̥̤͙̤̺͓̘͇͇̠̫͍̠̜̹̩̘̺̜̟̮̟̻͓͓̼͕͖͙͉̞̺̭̝̘̝̦̤̪͖͙͍͉̫̻̖̣̲͉͔̜̩͍͎̟̻̜̪̳͓͙̭̟͂̄̀̉͛́͑̆͊͗̈́̓̓̈́͊̍̓̇͌̎͒̃͊̏́̈͋̈́͊͊̈̑́͋͛̔͆̿͌́̃͐̏̄́͒̄͐̓̍̽̀̇͘̚̕̚̚̕̚͜͜͜͝ͅͅͅͅư̵̢̢̨̡̧̛̺̯̞͍̤̙͙̩̝͖̟̻̝̫̦̹̬̞͓͍̱̯̝̥͎̲̼̲̫̬̺͓͉̩̜̖̥̓͊́̿̓̌̈́̄̔̏̈̍̆́̀͂͆͋̾͑̎̆̐̈́͐̀͊͑̓̍̒̌̓̃̀͛̐̾́̏̈́̇̂́̀̋̌̈́̀̑̌͛̐͆̆̒͋̓͊̿̂͋̍͊̆̿̒͊̀̈́̉̒̋̋̌͂̍̔͒͒͛̍̔̀͌͂̀͗͆̏̊̈́͑̈́̎̅̏̒̌͌̒̈́̇̕̚̕̚͘̚̚̕͘͝͝͝͝͝ͅư̷̯̈́̊̈́͑́̈́͋̏̓̏͊̈́̑͐̌̽̋̈̓̏̅͛̉̌̽͊͌͂͛̑̓͒͆͌̓́͐̂͋̌̆̊͌͆̆̑̃̓̐̅͒̇̃̀̃̆͒̾̇́̈́̈́͐̍̇̓̌̄̎̅̋͆̉̓̐̉̿̋̽́̎̂̑̚̕̚̚̚͘̕̚͠͠͝͝͝h̵̢̧̧̨̛͓̺͇̺̝̤̤͍̞̦̻̯̳̣̳͈̺͈̩͎̩̙͚̲̳̟̪̖̭̠̥̖̖̖̩̳̻̫͚̤̬͓̺̗̼̲̯̙͉̻̳̦̲̖̦͙̣̲̺̖̝͖̬͉͈̝͔̗̰̬̠̙̠͔̫̭̘͇̭̮̦̰̫̹̭̹͎̦̼̠̱̣̘̪̣̝̰̤̹͖̤̟͐̾̂̌̋̾̈͂̿͛̒́̔̌͂͌͛̉͒̽̈͐̅́̓̃̐̔̔͗̈̓̓̇͆̒̀̓̔̎̀͑̐̋͑̒̄̊̆̊̂͛͑̓̔̐̾͋̋̄͊͒̉̄͒̆̚̕̕͘͘̚̕̕͝͝ͅͅ (&*)&€*?\"!';:'"));
        assert!(eval_err(""));
        assert!(eval_err("-"));
        assert!(eval_err("(())"));
        assert!(eval_err("1.849 + 1 - 3.51 -"));
    }

    #[test]
    fn one_plus_one() {
        assert_eq!(eval("1 + 1"), dec!(2));
    }

    #[test]
    fn addition_subtraction() {
        assert_eq!(eval("-1.67 + 3.483 + 48.492 - 0.003"), dec!(50.302));
    }

    #[test]
    fn order_of_operations() {
        assert_eq!(eval("1.67 + 3.483 * 48 ^ 3!"), dec!(42599146587.782));
        assert_eq!(eval("6 ! ^ 2 * 48492 + 0.003"), dec!(25138252800.003));
    }

    #[test]
    fn large_expression() {
        let expr = "-1 * (-998 + 45 ^ (44.7 - 44.125)) / (3.2 * 63.004 * (78.44 ^ -8 - 34.996))";
        assert_eq!(eval(expr).round_dp(6), dec!(-0.140182));
    }

    #[test]
    fn weird_operands() {
        assert_eq!(eval("4 × 3 - 8÷4"), dec!(10));
    }

    #[test]
    fn square_root() {
        assert_eq!(eval("√(√4.432 - √4)").round_dp(5), dec!(0.32439));
    }

    #[test]
    fn larger_expression() {
        let expr =
            "-ln(998 + (45 % 180) ^ (44.7 - 44.125)) / ln(3.2 * 63.004 * abs(78.44 ^ -8 - 34.996))";
        assert_eq!(eval(expr).round_dp(6), dec!(-0.780296));
    }

    #[test]
    fn cursed_expressions() {
        assert_eq!(eval("-- -- -- -6"), dec!(-6));
        assert_eq!(eval("-- -- -- --6"), dec!(6));
        assert_eq!(eval("---abs -- ln-- --6").round_dp(3), dec!(-1.792));
        assert_eq!(eval("abslnabsabsln--6").round_dp(3), dec!(0.583));
    }

    #[test]
    fn brackets() {
        assert_eq!(eval("(( (9-7)! - (-(8)) ))"), dec!(10));
    }
}
