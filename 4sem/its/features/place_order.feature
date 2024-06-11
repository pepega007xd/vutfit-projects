Feature: Place order

  Test if the user can create an order

  Scenario: Add product to shopping cart from featured items (7)
    Given User is on main page
    And Shopping cart is empty
    When User adds featured product to cart
    Then User should see notification
    And Shopping cart should contain one product

  Scenario: Add product to shopping cart from item details (8)
    Given User is on page with product details
    And Shopping cart is empty
    And product is in stock
    When User adds product to cart
    Then User should see notification
    And Shopping cart should contain one product

  Scenario: Display shopping cart content from popup (9)
    Given User is on main page
    And Shopping cart is not empty
    When User clicks on shopping cart
    Then User should see products in shopping cart

  Scenario: Display shopping cart content on separate page (10)
    Given User is on main page
    And Shopping cart is not empty
    When User opens shopping cart page
    Then User should see products in shopping cart

  Scenario: Change quantity inside shopping cart (11)
    Given Shopping cart is not empty
    And User is on shopping cart page
    When User updates quantity of product to "5"
    Then User should see notification
    Then User should see new quantity "5"

  Scenario: Remove item from shopping cart (12)
    Given Shopping cart is not empty
    And User is on shopping cart page
    When User removes product from shopping cart
    Then Shopping cart should be empty

  Scenario: Fill out personal information for order (13)
    Given User is on checkout page
    When User chooses guest checkout
    And User fills out required information
    And Users continues to payment method
    Then User should see notification

  Scenario: Confirm order (14)
    Given User is on checkout page
    And User has filled out personal information
    When User chooses shipping method
    And User chooses payment method
    And User confirms order
    Then User should see order success page
