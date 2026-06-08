--[[
  Quest 938: Panacea
  Level Required: 96
  NPC Start: 541, NPC Complete: 541
  Prerequisites: [937]
  Rewards: EXP=16537919
]]

local fsm = require('fsm_engine')

local quest_938 = fsm:new({
    id = 938,
    name = "Panacea",
    level_required = 96,
    npc_start = 541,
    npc_complete = 541,
    prerequisites = {937},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 938: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 541,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 938: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 938: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 329,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 938: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 541,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 938: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 938: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 541,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 938: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 938: Rewards given")
                self:give_rewards({
                    exp = 16537919,
                    items = {
                        { item_id = 408, count = 30 },
                        { item_id = 21000010, count = 10 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_938