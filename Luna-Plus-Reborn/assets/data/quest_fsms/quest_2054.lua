--[[
  Quest 2054: [Daily] Mutant Bugs 1
  Level Required: 104
  NPC Start: 412, NPC Complete: 412
  Rewards: EXP=6830879
]]

local fsm = require('fsm_engine')

local quest_2054 = fsm:new({
    id = 2054,
    name = "[Daily] Mutant Bugs 1",
    level_required = 104,
    npc_start = 412,
    npc_complete = 412,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 2054: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 412,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 2054: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 2054: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 329,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2054: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 412,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2054: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 2054: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 412,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 2054: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 2054: Rewards given")
                self:give_rewards({
                    exp = 6830879,
                    items = {
                        { item_id = 30000836, count = 2 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_2054