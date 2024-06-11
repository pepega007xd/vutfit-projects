Feature: Find products

  Test if we can find products through different pages

  Scenario: Find featured products on main page (1)
    Given User is on main page
    Then User should see featured products

  Scenario: Find products in category (2)
    Given User is on main page
    When User opens the entire category "Desktops"
    Then User should see products from category "Desktops"

  Scenario: Find products in subcategory (3)
    Given User is on main page
    When User opens category "Components"
    And User opens subcategory "Monitors"
    Then User should see products from subcategory "Monitors"

  Scenario: Display empty category (4)
    Given User is on main page
    When User opens category "Software"
    Then User should see empty category page

  Scenario: Search for product name (5)
    Given User is on main page
    When User searches "iPod"
    Then User should see product with name matching "iPod"

  Scenario: Search with filter (6)
    Given User is on main page
    When User searches "Samsung"
    And User sets search category to "Tablets"
    Then User should see product with name matching "Galaxy"
    # test that monitor is filtered out
    And User should not see product with name matching "SyncMaster"
