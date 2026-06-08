--[[
  Quest 2000: [Daily] Special Dishes
  Level Required: 40
  NPC Start: 411, NPC Complete: 411
  Rewards: EXP=50156
]]

local fsm = require('fsm_engine')

local quest_2000 = fsm:new({
    id = 2000,
    name = "[Daily] Special Dishes",
    level_required = 40,
    npc_start = 411,
    npc_complete = 411,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 2000: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 2000: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 2000: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 30,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2000: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2000: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 2000: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 2000: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 2000: Rewards given")
                self:give_rewards({
                    exp = 50156,
                    items = {
                        { item_id = 392, count = 10 },
                        { item_id = 30000704, count = 10 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_2000