--[[
  Quest 114: The Sword of Kings
  Level Required: 20
  NPC Start: 57, NPC Complete: 61
  Prerequisites: [105]
  Rewards: EXP=8296, Gold=3475
]]

local fsm = require('fsm_engine')

local quest_114 = fsm:new({
    id = 114,
    name = "The Sword of Kings",
    level_required = 20,
    npc_start = 57,
    npc_complete = 61,
    prerequisites = {105},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 114: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 61,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 114: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 114: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 34,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 114: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 61,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 114: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 57,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 114: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 114: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 61,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 114: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 114: Rewards given")
                self:give_rewards({
                    exp = 8296,
                    gold = 3475,
                    items = {
                        { item_id = 53, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_114