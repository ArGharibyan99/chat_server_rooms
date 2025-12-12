INSERT INTO users (username, password)
VALUES
    ('alice', 'pass123'),
    ('bob',   'pass123');

-- ===========================
-- Create sample room
-- ===========================

INSERT INTO rooms (name, owner_id)
VALUES ('General Chat', 1);

-- ===========================
-- Add users to the room
-- ===========================

INSERT INTO room_members (room_id, user_id)
VALUES
    (1, 1),
    (1, 2);

-- ===========================
-- Insert sample messages
-- ===========================

INSERT INTO messages (room_id, user_id, content)
VALUES
    (1, 1, 'Hello Bob!'),
    (1, 2, 'Hi Alice!');
