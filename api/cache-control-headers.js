export default function handler(request, response) {
  response.setHeader('Content-Type', 'application/json' );
  response.setHeader('Vercel-CDN-Cache-Control', 'max-age=3600');
  response.setHeader('CDN-Cache-Control', 'max-age=60');
  response.setHeader('Cache-Control', 'max-age=10');

  try {
    request.body;
  } catch (error) {
    return response.status(400).json({ error: 'My custom 400 error' });
  }

  return response.status(200).json({
    body: request.body,
    query: request.query,
    cookies: request.cookies,
    headers: request.headers
  });
}
