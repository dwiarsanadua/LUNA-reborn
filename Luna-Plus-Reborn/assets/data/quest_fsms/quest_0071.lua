--[[
  Quest 71: Equipment Delivery
  Level Required: 4
  NPC Start: 5, NPC Complete: 11
  Prerequisites: [70]
  Rewards: EXP=50
]]

local fsm = require('fsm_engine')

local quest_71 = fsm:new({
    id = 71,
    name = "Equipment Delivery",
    level_required = 4,
    npc_start = 5,
    npc_complete = 11,
    prerequisites = {70},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 71: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 11,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 71: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 71: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 11,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 71: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 5,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 71: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 71: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 11,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 71: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 71: Rewards given")
                self:give_rewards({
                    exp = 50,
                    items = {
                        { item_id = 21000006, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_71