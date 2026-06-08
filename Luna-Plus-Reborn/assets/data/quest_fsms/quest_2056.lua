--[[
  Quest 2056: [Daily] Mutant Bugs 3
  Level Required: 106
  NPC Start: 412, NPC Complete: 412
  Rewards: EXP=6744189
]]

local fsm = require('fsm_engine')

local quest_2056 = fsm:new({
    id = 2056,
    name = "[Daily] Mutant Bugs 3",
    level_required = 106,
    npc_start = 412,
    npc_complete = 412,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 2056: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 412,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 2056: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 2056: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 332,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2056: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 412,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2056: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 2056: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 412,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 2056: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 2056: Rewards given")
                self:give_rewards({
                    exp = 6744189,
                    items = {
                        { item_id = 30000836, count = 2 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_2056