--[[
  Quest 43: Chasing Shadows
  Level Required: 63
  NPC Start: 45, NPC Complete: 45
  Rewards: EXP=519714, Gold=18195
]]

local fsm = require('fsm_engine')

local quest_43 = fsm:new({
    id = 43,
    name = "Chasing Shadows",
    level_required = 63,
    npc_start = 45,
    npc_complete = 45,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 43: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 45,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 43: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 43: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 16,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 43: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 63,
                    count = 12,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 43: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 45,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 43: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 43: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 45,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 43: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 43: Rewards given")
                self:give_rewards({
                    exp = 519714,
                    gold = 18195,
                    items = {
                        { item_id = 30000026, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_43