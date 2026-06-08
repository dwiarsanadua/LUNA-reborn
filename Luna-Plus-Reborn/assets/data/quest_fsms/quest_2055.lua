--[[
  Quest 2055: [Daily] Mutant Bugs 2
  Level Required: 105
  NPC Start: 412, NPC Complete: 412
  Rewards: EXP=6208816
]]

local fsm = require('fsm_engine')

local quest_2055 = fsm:new({
    id = 2055,
    name = "[Daily] Mutant Bugs 2",
    level_required = 105,
    npc_start = 412,
    npc_complete = 412,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 2055: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 412,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 2055: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 2055: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 325,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2055: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 412,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2055: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 2055: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 412,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 2055: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 2055: Rewards given")
                self:give_rewards({
                    exp = 6208816,
                    items = {
                        { item_id = 30000836, count = 2 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_2055