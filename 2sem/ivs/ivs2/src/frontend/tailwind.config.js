/** @type {import('tailwindcss').Config} */
module.exports = {
  content: ["./index.html", "./src/**/*.rs"],
  theme: {
    extend: {
      padding: {
        "1/2": "50%",
        full: "100%"
      },
      colors: {
        'app-bg-start': '#1e293b',
        'app-bg-end': '#140527',
        'black':'#000000'
      }
    },
  },
  plugins: [ ]
}
