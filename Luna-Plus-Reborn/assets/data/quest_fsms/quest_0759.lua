--[[
  Quest 759: [Quest] Second Training
  Level Required: 0
  NPC Start: 401, NPC Complete: 408
  Prerequisites: [758]
  Rewards: EXP=100000, Gold=20000
]]

local fsm = require('fsm_engine')

local quest_759 = fsm:new({
    id = 759,
    name = "[Quest] Second Training",
    level_required = 0,
    npc_start = 401,
    npc_complete = 408,
    prerequisites = {758},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 759: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 401,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 759: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 759: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 56,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 759: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 401,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 759: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 408,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 759: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 759: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 408,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 759: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 759: Rewards given")
                self:give_rewards({
                    exp = 100000,
                    gold = 20000,
                    items = {
                        { item_id = 13000009, count = 2 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_759