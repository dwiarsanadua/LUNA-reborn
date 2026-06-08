--[[
  Quest 356: [Party] Wicked Flames
  Level Required: 40
  NPC Start: 59, NPC Complete: 59
  Prerequisites: [355]
  Rewards: EXP=95297, Gold=11000
]]

local fsm = require('fsm_engine')

local quest_356 = fsm:new({
    id = 356,
    name = "[Party] Wicked Flames",
    level_required = 40,
    npc_start = 59,
    npc_complete = 59,
    prerequisites = {355},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 356: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 59,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 356: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 356: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 160,
                    count = 60,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 356: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 161,
                    count = 60,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 356: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 59,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 356: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 356: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 59,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 356: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 356: Rewards given")
                self:give_rewards({
                    exp = 95297,
                    gold = 11000,
                    items = {
                        { item_id = 21000097, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_356