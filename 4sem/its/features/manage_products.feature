Feature: Manage products

  Test if admin can display and change information about products

  Scenario: Display products (15)
    Given Admin is on administration dashboard
    When Admin opens page "Products"
    Then Admin should see list of products

  Scenario: Filter displayed products (16)
    Given Admin is on products page
    When Admin sets quantity filter to "1000"
    Then Admin should see list of products with quantity "1000"

  Scenario: Add new product (17)
    Given Admin is on products page
    # zero is for product to be on the first page
    When Admin creates new product with name "0_new_product"
    And Admin sets required product info
    And Admin saves and closes the product
    Then Page should contain product with name "0_new_product"

  Scenario: Change product name (19)
    Given Admin is on products page
    When Admin opens product for editing
    # zero is for product to be on the first page
    And Admin changes product name to "0_new_name"
    And Admin saves and closes the product
    Then Page should contain product with name "0_new_name"

  Scenario: Delete product (18)
    Given Admin is on products page
    When Admin selects a product
    And Admin deletes selected products
    Then Admin should see notification

  Scenario: Change product quantity (20)
    Given Admin is on products page
    When Admin opens product for editing
    And Admin changes product quantity to "123"
    And Admin saves and closes the product
    Then Product should have quantity "123"
