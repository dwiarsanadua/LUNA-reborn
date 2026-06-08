--[[
  Quest 480: [Halloween Event]Trick or Treat!
  Level Required: 1
  NPC Start: 137, NPC Complete: 141
]]

local fsm = require('fsm_engine')

local quest_480 = fsm:new({
    id = 480,
    name = "[Halloween Event]Trick or Treat!",
    level_required = 1,
    npc_start = 137,
    npc_complete = 141,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 480: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 137,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 480: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 480: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 137,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 480: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 138,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 480: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 139,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 480: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 140,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 480: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 141,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 480: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 480: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 141,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 480: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 480: Rewards given")
                self:give_rewards({
                    items = {
                        { item_id = 21000516, count = 1 },
                        { item_id = 21000517, count = 1 },
                        { item_id = 21000518, count = 1 },
                        { item_id = 21000519, count = 2 },
                        { item_id = 21000520, count = 2 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_480