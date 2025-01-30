/** @type {import('next').NextConfig} */
const nextConfig = {
  experimental: {
    serverActions: {
      allowedOrigins: [
        "localhost:3000",
        "localhost:3001",
        "fitstagram.kentus.eu",
        "fitstagram2.kentus.eu",
        "fitstagram-test.kentus.eu" 
      ]
    }
  }
}
export default nextConfig;
