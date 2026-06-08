--[[
  Quest 2049: [Daily] Giant Garden 2
  Level Required: 99
  NPC Start: 412, NPC Complete: 412
  Rewards: EXP=4820880
]]

local fsm = require('fsm_engine')

local quest_2049 = fsm:new({
    id = 2049,
    name = "[Daily] Giant Garden 2",
    level_required = 99,
    npc_start = 412,
    npc_complete = 412,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 2049: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 412,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 2049: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 2049: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 322,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2049: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 412,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2049: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 2049: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 412,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 2049: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 2049: Rewards given")
                self:give_rewards({
                    exp = 4820880,
                    items = {
                        { item_id = 30000836, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_2049