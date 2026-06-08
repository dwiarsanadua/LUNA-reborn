--[[
  Quest 2029: [Daily] Lamia Lament
  Level Required: 79
  NPC Start: 411, NPC Complete: 411
  Rewards: EXP=998293
]]

local fsm = require('fsm_engine')

local quest_2029 = fsm:new({
    id = 2029,
    name = "[Daily] Lamia Lament",
    level_required = 79,
    npc_start = 411,
    npc_complete = 411,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 2029: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 2029: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 2029: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 73,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2029: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2029: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 2029: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 2029: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 2029: Rewards given")
                self:give_rewards({
                    exp = 998293,
                    items = {
                        { item_id = 30000835, count = 2 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_2029