--[[
  Quest 2014: [Daily] Shadow of Resentment
  Level Required: 64
  NPC Start: 411, NPC Complete: 411
  Rewards: EXP=359143
]]

local fsm = require('fsm_engine')

local quest_2014 = fsm:new({
    id = 2014,
    name = "[Daily] Shadow of Resentment",
    level_required = 64,
    npc_start = 411,
    npc_complete = 411,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 2014: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 2014: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 2014: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 63,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2014: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2014: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 2014: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 2014: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 2014: Rewards given")
                self:give_rewards({
                    exp = 359143,
                    items = {
                        { item_id = 30000835, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_2014