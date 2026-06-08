--[[
  Quest 2007: [Daily] Jenebeu Subjugation
  Level Required: 54
  NPC Start: 411, NPC Complete: 411
  Rewards: EXP=124375
]]

local fsm = require('fsm_engine')

local quest_2007 = fsm:new({
    id = 2007,
    name = "[Daily] Jenebeu Subjugation",
    level_required = 54,
    npc_start = 411,
    npc_complete = 411,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 2007: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 2007: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 2007: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 146,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2007: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2007: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 2007: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 2007: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 2007: Rewards given")
                self:give_rewards({
                    exp = 124375,
                    items = {
                        { item_id = 30000704, count = 10 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_2007