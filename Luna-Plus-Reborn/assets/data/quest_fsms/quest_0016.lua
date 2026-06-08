--[[
  Quest 16: Bears! The Number One Threat!
  Level Required: 20
  NPC Start: 18, NPC Complete: 18
  Rewards: EXP=5392, Gold=1390
]]

local fsm = require('fsm_engine')

local quest_16 = fsm:new({
    id = 16,
    name = "Bears! The Number One Threat!",
    level_required = 20,
    npc_start = 18,
    npc_complete = 18,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 16: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 18,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 16: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 16: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 4,
                    count = 8,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 16: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 18,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 16: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 16: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 18,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 16: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 16: Rewards given")
                self:give_rewards({
                    exp = 5392,
                    gold = 1390,
                    items = {
                        { item_id = 21000008, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_16